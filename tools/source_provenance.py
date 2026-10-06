"""Check explicitly declared source provenance before any compilation.

This verifies bytes, not license eligibility or an inferred original identity.
Declarations remain optional for independently reconstructed project code.
"""
import hashlib
import json
import re
from pathlib import Path

from analyze import ROOT


def validate_sources(functions, root=ROOT):
    """Validate all declarations, including every pinned package dependency."""
    root = Path(root).resolve()
    digests = {}
    packages = {}
    checked = []

    def resolve(value):
        if not isinstance(value, str) or not value or Path(value).is_absolute():
            raise ValueError("Source provenance paths must be workspace-relative")
        path = (root / value).resolve()
        try:
            path.relative_to(root)
        except ValueError as error:
            raise ValueError(f"Source provenance path escapes workspace: {value}") from error
        return path

    def check(value, expected):
        if not isinstance(expected, str) or not re.fullmatch(r"[0-9a-f]{64}", expected):
            raise ValueError(f"Malformed source SHA256 declaration: {value}")
        path = resolve(value)
        if path not in digests:
            if not path.is_file():
                raise ValueError(f"Missing declared source dependency: {value}")
            digests[path] = hashlib.sha256(path.read_bytes()).hexdigest()
        if digests[path] != expected:
            raise ValueError(f"Declared source fingerprint mismatch: {value}")
        return path

    for function in functions:
        source = function.get("source")
        expected = function.get("upstream_source_sha256")
        if "upstream_source_sha256" in function:
            check(source, expected)
        dependencies = function.get("source_dependencies", [])
        if not isinstance(dependencies, list):
            raise ValueError("Source dependencies must be a list")
        for dependency in dependencies:
            if not isinstance(dependency, dict) or not {"path", "sha256"} <= dependency.keys():
                raise ValueError("Source dependency needs an explicit path and SHA256")
            check(dependency["path"], dependency["sha256"])
        if "source_provenance_manifest" in function:
            manifest = resolve(function["source_provenance_manifest"])
            if manifest not in packages:
                if not manifest.is_file():
                    raise ValueError(f"Missing source provenance manifest: {manifest}")
                package = json.loads(manifest.read_text(encoding="utf-8"))
                if not isinstance(package, dict) or not isinstance(package.get("files"), list):
                    raise ValueError("Source provenance manifest needs a files list")
                entries = {}
                for entry in package["files"]:
                    if not isinstance(entry, dict) or not {"path", "sha256"} <= entry.keys():
                        raise ValueError("Package dependency needs an explicit path and SHA256")
                    path = check(entry["path"], entry["sha256"])
                    if path in entries:
                        raise ValueError(f"Duplicate source provenance package path: {entry['path']}")
                    entries[path] = entry["sha256"]
                packages[manifest] = entries
            if expected is None or packages[manifest].get(resolve(source)) != expected:
                raise ValueError("Provenance package must pin the function source to its declared SHA256")
        if ("upstream_source_sha256" in function or dependencies
                or "source_provenance_manifest" in function):
            checked.append(function.get("name", source))
    return checked
