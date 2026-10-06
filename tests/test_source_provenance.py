"""Corruption and declaration-boundary checks for pinned runtime source."""
import hashlib
import json
import sys
import tempfile
import unittest
from unittest import mock
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from source_provenance import validate_sources


class SourceProvenanceTests(unittest.TestCase):
    def setUp(self):
        self.folder = tempfile.TemporaryDirectory()
        self.addCleanup(self.folder.cleanup)
        self.root = Path(self.folder.name)
        self.source = self.root / "source.c"
        self.source.write_bytes(b"int function(void) { return 1; }\n")
        self.header = self.root / "types.h"
        self.header.write_bytes(b"typedef unsigned int word;\n")
        self.source_sha = hashlib.sha256(self.source.read_bytes()).hexdigest()
        self.header_sha = hashlib.sha256(self.header.read_bytes()).hexdigest()
        self.function = {"name": "function", "source": "source.c",
                         "upstream_source_sha256": self.source_sha}

    def package(self, entries=None):
        if entries is None:
            entries = [{"path": "source.c", "sha256": self.source_sha},
                       {"path": "types.h", "sha256": self.header_sha, "kind": "generated"}]
        (self.root / "provenance.json").write_text(json.dumps({"files": entries}))
        self.function["source_provenance_manifest"] = "provenance.json"

    def test_undeclared_project_source_stays_optional(self):
        self.assertEqual(validate_sources([{"name": "project", "source": "not-created.c"}], self.root), [])

    def test_complete_package_and_generated_dependency_pass(self):
        self.package()
        self.assertEqual(validate_sources([self.function, self.function], self.root), ["function", "function"])

    def test_source_corruption_is_rejected(self):
        self.source.write_bytes(b"int function(void) { return 2; }\n")
        with self.assertRaisesRegex(ValueError, "fingerprint mismatch: source.c"):
            validate_sources([self.function], self.root)

    def test_missing_source_is_rejected(self):
        self.source.unlink()
        with self.assertRaisesRegex(ValueError, "Missing declared"):
            validate_sources([self.function], self.root)

    def test_package_header_corruption_and_missing_are_rejected(self):
        self.package()
        self.header.write_bytes(b"typedef unsigned long long word;\n")
        with self.assertRaisesRegex(ValueError, "fingerprint mismatch: types.h"):
            validate_sources([self.function], self.root)
        self.header.unlink()
        with self.assertRaisesRegex(ValueError, "Missing declared.*types.h"):
            validate_sources([self.function], self.root)

    def test_missing_package_and_missing_source_entry_are_rejected(self):
        self.function["source_provenance_manifest"] = "missing.json"
        with self.assertRaisesRegex(ValueError, "Missing source provenance manifest"):
            validate_sources([self.function], self.root)
        self.package([{"path": "types.h", "sha256": self.header_sha}])
        with self.assertRaisesRegex(ValueError, "must pin the function source"):
            validate_sources([self.function], self.root)

    def test_package_source_requires_matching_function_declaration(self):
        self.package()
        del self.function["upstream_source_sha256"]
        with self.assertRaisesRegex(ValueError, "must pin the function source"):
            validate_sources([self.function], self.root)

    def test_adapter_dependency_is_checked_independently(self):
        self.function["source_dependencies"] = [{"path": "types.h", "sha256": self.header_sha,
                                                "modified": True, "evidence": "explicit adapter"}]
        self.assertEqual(validate_sources([self.function], self.root), ["function"])
        self.header.write_bytes(b"changed adapter")
        with self.assertRaisesRegex(ValueError, "fingerprint mismatch: types.h"):
            validate_sources([self.function], self.root)

    def test_duplicate_and_conflicting_package_entries_are_rejected(self):
        for changed_sha in (self.source_sha, self.header_sha):
            self.package([{"path": "source.c", "sha256": self.source_sha},
                          {"path": "source.c", "sha256": changed_sha}])
            with self.subTest(sha=changed_sha), self.assertRaises(ValueError):
                validate_sources([self.function], self.root)

    def test_conflicting_function_source_declarations_are_rejected(self):
        other = dict(self.function, upstream_source_sha256=self.header_sha)
        with self.assertRaisesRegex(ValueError, "fingerprint mismatch"):
            validate_sources([self.function, other], self.root)

    def test_malformed_hash_and_dependency_shapes_are_rejected(self):
        for value in (None, "", "abc", self.source_sha.upper(), 12):
            with self.subTest(value=value), self.assertRaisesRegex(ValueError, "Malformed source SHA256"):
                validate_sources([dict(self.function, upstream_source_sha256=value)], self.root)
        for value in (None, {}, [None], [{"path": "types.h"}]):
            with self.subTest(value=value), self.assertRaises(ValueError):
                validate_sources([dict(self.function, source_dependencies=value)], self.root)
        self.package()
        (self.root / "provenance.json").write_text(json.dumps({"files": "invalid"}))
        with self.assertRaisesRegex(ValueError, "files list"):
            validate_sources([self.function], self.root)

    def test_absolute_and_escaping_dependency_paths_are_rejected(self):
        for value in (str(self.source.resolve()), "../outside.c"):
            with self.subTest(value=value), self.assertRaises(ValueError):
                validate_sources([dict(self.function, source=value)], self.root)

    def test_verifier_checks_all_declarations_before_compiler_runs(self):
        import verify
        self.header.write_bytes(b"corrupted last declared input")
        last = {"name": "last", "source": "types.h", "upstream_source_sha256": self.header_sha}
        with mock.patch.object(sys, "argv", ["verify.py"]), \
                mock.patch.object(verify, "validated_elf", return_value=({}, b"")), \
                mock.patch.object(verify, "manifests", return_value=[self.function, last]), \
                mock.patch.object(verify, "validate_sources", side_effect=lambda f: validate_sources(f, self.root)), \
                mock.patch.object(verify.subprocess, "run") as process:
            with self.assertRaisesRegex(ValueError, "fingerprint mismatch: types.h"):
                verify.main()
            process.assert_not_called()

    def test_build_checks_declarations_before_any_subprocess(self):
        import build
        self.source.write_bytes(b"corrupted build input")
        with mock.patch.object(build, "manifests", return_value=[self.function]), \
                mock.patch.object(build, "validate_sources", side_effect=lambda f: validate_sources(f, self.root)), \
                mock.patch.object(build.subprocess, "run") as process:
            with self.assertRaisesRegex(ValueError, "fingerprint mismatch: source.c"):
                build.main()
            process.assert_not_called()


if __name__ == "__main__":
    unittest.main()
