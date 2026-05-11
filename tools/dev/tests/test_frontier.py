import importlib.util
import sys
import tempfile
import types
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]


def load_module(name, path):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


frontier = load_module("frontier", REPO_ROOT / "tools" / "dev" / "frontier.py")


class FrontierTests(unittest.TestCase):
    def make_args(self, **overrides):
        args = types.SimpleNamespace(no_menu=False, overlays_only=False, include_rodata=False)
        for key, value in overrides.items():
            setattr(args, key, value)
        return args

    def test_collect_frontiers_reads_stub_metadata(self):
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)
            source = root / "src" / "BATTLE" / "BATTLE.PRG" / "sample.c"
            asm = root / "build" / "src" / "BATTLE" / "BATTLE.PRG" / "nonmatchings" / "sample"
            source.parent.mkdir(parents=True)
            asm.mkdir(parents=True)

            source.write_text(
                'INCLUDE_ASM("build/src/BATTLE/BATTLE.PRG/nonmatchings/sample", func_800C0000);\n'
                'INCLUDE_RODATA("build/src/BATTLE/BATTLE.PRG/nonmatchings/sample", D_80060000);\n'
            )
            (asm / "func_800C0000.s").write_text(
                "/* Handwritten function */\nnonmatching func_800C0000, 0x44\n"
            )

            frontiers = frontier.collect_frontiers(
                root,
                self.make_args(overlays_only=True, include_rodata=True),
            )

            self.assertEqual(len(frontiers), 1)
            self.assertEqual(frontiers[0].path, "src/BATTLE/BATTLE.PRG/sample.c")
            self.assertEqual(frontiers[0].asm_count, 1)
            self.assertEqual(frontiers[0].rodata_count, 1)
            self.assertEqual(frontiers[0].tags, ["gameplay"])
            self.assertEqual(frontiers[0].stubs[0].size, "0x44")
            self.assertTrue(frontiers[0].stubs[0].handwritten)

    def test_no_menu_excludes_menu_frontiers(self):
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)
            source = root / "src" / "MENU" / "MENUF.PRG" / "sample.c"
            source.parent.mkdir(parents=True)
            source.write_text(
                'INCLUDE_ASM("build/src/MENU/MENUF.PRG/nonmatchings/sample", func_80100000);\n'
            )

            frontiers = frontier.collect_frontiers(root, self.make_args(no_menu=True))

            self.assertEqual(frontiers, [])

    def test_read_asm_metadata_ignores_paths_outside_repo(self):
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)

            size, handwritten = frontier.read_asm_metadata(root, "../outside", "func_80000000")

            self.assertEqual(size, "")
            self.assertFalse(handwritten)

    def test_native_tags_can_combine_render_and_gameplay(self):
        tags = frontier.native_tags("src/BATTLE/BATTLE.PRG/libgpu/sample.c")

        self.assertEqual(tags, ["render", "gameplay"])

    def test_native_tags_identifies_menu_ui_and_unknown_paths(self):
        self.assertEqual(frontier.native_tags("src/MENU/MAINMENU.PRG/sample.c"), ["ui"])
        self.assertEqual(frontier.native_tags("src/SMALL/misc/sample.c"), ["unknown"])

    def test_include_rodata_controls_rodata_only_frontiers(self):
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)
            source = root / "src" / "TITLE" / "TITLE.PRG" / "sample.c"
            source.parent.mkdir(parents=True)
            source.write_text(
                'INCLUDE_RODATA("build/src/TITLE/TITLE.PRG/nonmatchings/sample", D_80060000);\n'
            )

            default_frontiers = frontier.collect_frontiers(root, self.make_args())
            rodata_frontiers = frontier.collect_frontiers(
                root,
                self.make_args(include_rodata=True),
            )

            self.assertEqual(default_frontiers, [])
            self.assertEqual(len(rodata_frontiers), 1)
            self.assertEqual(rodata_frontiers[0].asm_count, 0)
            self.assertEqual(rodata_frontiers[0].rodata_count, 1)

    def test_overlays_only_excludes_non_overlay_frontiers(self):
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)
            source = root / "src" / "SMALL" / "misc" / "sample.c"
            source.parent.mkdir(parents=True)
            source.write_text(
                'INCLUDE_ASM("build/src/SMALL/misc/nonmatchings/sample", func_80010000);\n'
            )

            frontiers = frontier.collect_frontiers(root, self.make_args(overlays_only=True))

            self.assertEqual(frontiers, [])

    def test_missing_asm_metadata_keeps_stub_without_size(self):
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)
            source = root / "src" / "BATTLE" / "BATTLE.PRG" / "sample.c"
            source.parent.mkdir(parents=True)
            source.write_text(
                'INCLUDE_ASM("build/src/BATTLE/BATTLE.PRG/nonmatchings/sample", func_800C0000);\n'
            )

            frontiers = frontier.collect_frontiers(root, self.make_args())

            self.assertEqual(len(frontiers), 1)
            self.assertEqual(frontiers[0].stubs[0].size, "")
            self.assertFalse(frontiers[0].stubs[0].handwritten)

    def test_frontiers_sort_by_asm_rodata_then_path(self):
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)
            first = root / "src" / "BATTLE" / "BATTLE.PRG" / "a.c"
            second = root / "src" / "BATTLE" / "BATTLE.PRG" / "b.c"
            third = root / "src" / "TITLE" / "TITLE.PRG" / "c.c"
            for source in (first, second, third):
                source.parent.mkdir(parents=True, exist_ok=True)
            first.write_text(
                'INCLUDE_ASM("build/src/BATTLE/BATTLE.PRG/nonmatchings/a", func_800C0000);\n'
            )
            second.write_text(
                'INCLUDE_ASM("build/src/BATTLE/BATTLE.PRG/nonmatchings/b", func_800C0000);\n'
                'INCLUDE_ASM("build/src/BATTLE/BATTLE.PRG/nonmatchings/b", func_800C0004);\n'
            )
            third.write_text(
                'INCLUDE_ASM("build/src/TITLE/TITLE.PRG/nonmatchings/c", func_80070000);\n'
                'INCLUDE_RODATA("build/src/TITLE/TITLE.PRG/nonmatchings/c", D_80070000);\n'
            )

            frontiers = frontier.collect_frontiers(root, self.make_args(include_rodata=True))

            self.assertEqual(
                [item.path for item in frontiers],
                [
                    "src/BATTLE/BATTLE.PRG/a.c",
                    "src/TITLE/TITLE.PRG/c.c",
                    "src/BATTLE/BATTLE.PRG/b.c",
                ],
            )


if __name__ == "__main__":
    unittest.main()
