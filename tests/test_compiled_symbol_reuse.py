"""A repeated source definition may omit only its own old entry binding."""
import copy
import hashlib
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import verify


class CompiledSymbolReuse(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        (self.root / "config/symbols").mkdir(parents=True)
        (self.root / "config/functions").mkdir()
        (self.root / "src").mkdir()
        source = b"void source_method(void) {}\n"
        (self.root / "src/method.c").write_bytes(source)
        digest = hashlib.sha256(source).hexdigest()
        provenance = {"schema_version": 1, "files": [
            {"path": "src/method.c", "sha256": digest}]}
        (self.root / "src/provenance.json").write_text(json.dumps(provenance))
        self.seed = dict(name="old_entry", address="0x1000", size=8,
                         source="src/method.c", source_kind="c",
                         compiled_symbol="source_method", compiler_profile="gcc29",
                         compile_flags=["-O2", "-ffunction-sections"],
                         original_sha256="a" * 64, reviewed=True,
                         upstream_source_sha256=digest,
                         source_provenance_manifest="src/provenance.json")
        self.copy = copy.deepcopy(self.seed)
        self.copy.update(name="new_entry", address="0x2000",
                         reuse_compiled_symbol="old_entry")
        self.bindings = dict(source_method="0x1000", old_entry="0x1000",
                             new_entry="0x2000", helper="0x3000", global_data="0x4000")
        self.write_inputs()
        self.addCleanup(patch.stopall)
        patch.object(verify, "ROOT", self.root).start()

    def write_inputs(self):
        (self.root / "config/symbols/test.json").write_text(
            json.dumps({"symbols": self.bindings}))
        (self.root / "config/functions/test.json").write_text(
            json.dumps({"functions": [self.seed]}))

    def test_only_selected_definition_binding_is_removed(self):
        global_map = verify.symbol_bindings({})
        result = verify.symbol_bindings(self.copy)
        self.assertEqual(result, {k: v for k, v in global_map.items()
                                  if k != "source_method"})
        self.assertEqual(verify.symbol_bindings({}), global_map)
        self.assertEqual(self.bindings["source_method"], "0x1000")

    def test_unknown_unreviewed_and_chained_seeds_are_rejected(self):
        for mode in ("unknown", "unreviewed", "chained"):
            with self.subTest(mode=mode):
                self.seed.pop("reuse_compiled_symbol", None)
                self.seed["reviewed"] = mode != "unreviewed"
                f = copy.deepcopy(self.copy)
                if mode == "unknown":
                    f["reuse_compiled_symbol"] = "missing"
                if mode == "chained":
                    self.seed["reuse_compiled_symbol"] = "earlier"
                self.write_inputs()
                with self.assertRaises(ValueError):
                    verify.symbol_bindings(f)

    def test_changed_source_recipe_or_original_body_is_rejected(self):
        changes = dict(source="src/other.c", source_kind="cxx",
                       compiled_symbol="other", compiler_profile="gcc323",
                       compile_flags=["-O3"], size=12,
                       original_sha256="b" * 64,
                       source_provenance_manifest="src/other.json")
        for key, value in changes.items():
            with self.subTest(key=key):
                f = copy.deepcopy(self.copy)
                f[key] = value
                with self.assertRaises(ValueError):
                    verify.symbol_bindings(f)

    def test_wrong_entry_bindings_and_source_mutation_are_rejected(self):
        for key in ("new_entry", "source_method"):
            with self.subTest(binding=key):
                saved = self.bindings[key]
                self.bindings[key] = "0x9000"
                self.write_inputs()
                with self.assertRaises(ValueError):
                    verify.symbol_bindings(self.copy)
                self.bindings[key] = saved
        self.write_inputs()
        (self.root / "src/method.c").write_bytes(b"changed source\n")
        with self.assertRaises(ValueError):
            verify.symbol_bindings(self.copy)

    def test_legacy_matching_seed_with_recorded_review_is_supported(self):
        self.seed.pop("reviewed")
        self.seed.update(status="matched", independent_review="Whole original/source/link reviewed.")
        self.write_inputs()
        self.assertNotIn("source_method", verify.symbol_bindings(self.copy))
        self.seed["independent_review"] = ""
        self.write_inputs()
        with self.assertRaises(ValueError):
            verify.symbol_bindings(self.copy)

    def test_legacy_absent_manifest_keeps_full_source_hash_gate(self):
        self.seed.pop("source_provenance_manifest")
        self.copy.pop("source_provenance_manifest")
        self.write_inputs()
        self.assertNotIn("source_method", verify.symbol_bindings(self.copy))
        self.assertNotIn("source_provenance_manifest", self.seed)
        self.seed.pop("upstream_source_sha256")
        self.copy.pop("upstream_source_sha256")
        self.write_inputs()
        with self.assertRaises(ValueError):
            verify.symbol_bindings(self.copy)


if __name__ == "__main__":
    unittest.main()
