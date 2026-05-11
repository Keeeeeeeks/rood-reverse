import sys
import json
from pathlib import Path


def load_category_mappings(categories_path: Path):
    with open(categories_path, "r") as cat_file:
        categories = json.load(cat_file)

    # Extract the category_mappings object from the categories list
    for item in categories:
        if "category_mappings" in item:
            return item["category_mappings"]

    raise ValueError("No category_mappings found in categories.json")


def load_complete_units(complete_units_path: Path):
    with open(complete_units_path, "r") as complete_file:
        complete_units = json.load(complete_file)

    if not isinstance(complete_units, list):
        raise ValueError(f"{complete_units_path} must contain a JSON list")
    if not all(isinstance(unit, str) for unit in complete_units):
        raise ValueError(f"{complete_units_path} must contain only string unit names")

    return set(complete_units)


def get_name_and_category(base_path: Path, category_mappings: dict):
    parts = base_path.parts
    for i, part in enumerate(parts):
        if part.endswith(".PRG") or part == "SLUS_010.40":
            name = Path(*parts[i:]).with_suffix("")
            old_category = part
            # Map the old category to the new prefixed category
            new_category = category_mappings.get(part, part)
            # Get the supercategory (prefix before the dot)
            supercategory = new_category.split(".")[0] if "." in new_category else new_category
            return name, [supercategory, new_category]
    raise ValueError(f"No valid category in path: {base_path}")


def main(basepath: Path, targetpath: Path, categories_path: Path):
    categories_path = categories_path.resolve()
    category_mappings = load_category_mappings(categories_path)
    repo_root = categories_path.parent.parent.parent
    complete_units = load_complete_units(repo_root / "config" / "complete_units.json")

    units = []
    for base_path in (basepath / "src").rglob("*.o"):
        name, progress_categories = get_name_and_category(base_path, category_mappings)
        target_path = targetpath / base_path.relative_to(basepath)
        unit = {
            "name": str(name),
            "target_path": str(target_path),
            "metadata": {
                "progress_categories": progress_categories,
                "complete": str(name) in complete_units
            }
        }
        if (
            base_path.name not in {"32154.o", "2EA3C.o", "38C1C.o", "6E644.o"}
            and not base_path.name.endswith((".data.o", ".rodata.o", ".bss.o"))
        ):
            unit["base_path"] = str(base_path)
        units.append(unit)

    with open(categories_path, "r") as cat_file:
        categories = json.load(cat_file)

    # Filter out category_mappings from the output
    filtered_categories = [item for item in categories if "category_mappings" not in item]

    with open("objdiff.json", "w") as f:
        json.dump({"units": units, "progress_categories": filtered_categories}, f, indent=2)


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python objdiff_config.py <basepath> <targetpath> <categories.json>")
        sys.exit(1)
    main(Path(sys.argv[1]), Path(sys.argv[2]), Path(sys.argv[3]))
