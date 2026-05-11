import importlib.util
import json
import os
import sys
import tempfile
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]


def load_module(name, path):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


objdiff_config = load_module(
    "objdiff_config", REPO_ROOT / "tools" / "dev" / "objdiff_config.py"
)


class ObjdiffConfigTests(unittest.TestCase):
    def test_load_complete_units_requires_json_list(self):
        with tempfile.TemporaryDirectory() as temp_dir:
            path = Path(temp_dir) / "complete_units.json"
            path.write_text(json.dumps({"complete": ["BATTLE.PRG/58578"]}))

            with self.assertRaises(ValueError):
                objdiff_config.load_complete_units(path)

    def test_load_complete_units_requires_string_list(self):
        with tempfile.TemporaryDirectory() as temp_dir:
            path = Path(temp_dir) / "complete_units.json"
            path.write_text(json.dumps(["BATTLE.PRG/58578", 3]))

            with self.assertRaises(ValueError):
                objdiff_config.load_complete_units(path)

    def test_get_name_and_category_uses_unmapped_prg_as_fallback(self):
        name, categories = objdiff_config.get_name_and_category(
            Path("build/src/FOO/TEST.PRG/1234.o"),
            {},
        )

        self.assertEqual(str(name), "TEST.PRG/1234")
        self.assertEqual(categories, ["TEST", "TEST.PRG"])

    def test_get_name_and_category_supports_slus_binary_paths(self):
        name, categories = objdiff_config.get_name_and_category(
            Path("build/src/SLUS_010.40/main.o"),
            {"SLUS_010.40": "main.SLUS_010.40"},
        )

        self.assertEqual(str(name), "SLUS_010.40/main")
        self.assertEqual(categories, ["main", "main.SLUS_010.40"])

    def test_get_name_and_category_rejects_unknown_object_paths(self):
        with self.assertRaises(ValueError):
            objdiff_config.get_name_and_category(Path("build/src/misc/1234.o"), {})

    def test_main_generates_units_from_complete_units_config(self):
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)
            categories_path = root / "tools" / "dev" / "categories.json"
            complete_units_path = root / "config" / "complete_units.json"
            build_root = root / "build"
            battle_object = build_root / "src" / "BATTLE" / "BATTLE.PRG" / "58578.o"
            data_object = build_root / "src" / "BATTLE" / "BATTLE.PRG" / "58578.data.o"
            old_cwd = Path.cwd()

            categories_path.parent.mkdir(parents=True)
            complete_units_path.parent.mkdir(parents=True)
            battle_object.parent.mkdir(parents=True)
            battle_object.touch()
            data_object.touch()
            categories_path.write_text(
                json.dumps(
                    [
                        {"category_mappings": {"BATTLE.PRG": "battle.BATTLE.PRG"}},
                        {"id": "battle", "name": "Battle"},
                    ]
                )
            )
            complete_units_path.write_text(json.dumps(["BATTLE.PRG/58578"]))

            try:
                os.chdir(root)
                objdiff_config.main(build_root, build_root, categories_path)
            finally:
                os.chdir(old_cwd)

            output = json.loads((root / "objdiff.json").read_text())
            units = {unit["name"]: unit for unit in output["units"]}

            self.assertTrue(units["BATTLE.PRG/58578"]["metadata"]["complete"])
            self.assertEqual(
                units["BATTLE.PRG/58578"]["metadata"]["progress_categories"],
                ["battle", "battle.BATTLE.PRG"],
            )
            self.assertIn("base_path", units["BATTLE.PRG/58578"])
            self.assertNotIn("base_path", units["BATTLE.PRG/58578.data"])
            self.assertEqual(output["progress_categories"], [{"id": "battle", "name": "Battle"}])

    def test_main_omits_base_path_for_special_object_names(self):
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)
            categories_path = root / "tools" / "dev" / "categories.json"
            complete_units_path = root / "config" / "complete_units.json"
            build_root = root / "build"
            special_object = build_root / "src" / "BATTLE" / "BATTLE.PRG" / "32154.o"
            old_cwd = Path.cwd()

            categories_path.parent.mkdir(parents=True)
            complete_units_path.parent.mkdir(parents=True)
            special_object.parent.mkdir(parents=True)
            special_object.touch()
            categories_path.write_text(
                json.dumps(
                    [
                        {"category_mappings": {"BATTLE.PRG": "battle.BATTLE.PRG"}},
                        {"id": "battle", "name": "Battle"},
                    ]
                )
            )
            complete_units_path.write_text(json.dumps([]))

            try:
                os.chdir(root)
                objdiff_config.main(build_root, build_root, categories_path)
            finally:
                os.chdir(old_cwd)

            output = json.loads((root / "objdiff.json").read_text())
            unit = output["units"][0]

            self.assertEqual(unit["name"], "BATTLE.PRG/32154")
            self.assertNotIn("base_path", unit)
            self.assertEqual(
                unit["target_path"],
                str(build_root / "src" / "BATTLE" / "BATTLE.PRG" / "32154.o"),
            )


if __name__ == "__main__":
    unittest.main()
