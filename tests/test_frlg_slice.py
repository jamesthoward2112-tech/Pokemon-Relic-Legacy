import importlib.util
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SCRIPT = ROOT / "tools/prl/validate_frlg_kanto_slice.py"

def test_frlg_foundation_slice():
    spec = importlib.util.spec_from_file_location("prl_frlg_validator", SCRIPT)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    mod.main()
