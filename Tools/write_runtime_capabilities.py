"""Called only after the packaged MIXR plugin passes the native load/ABI/evaluation test."""
from pathlib import Path
import sys

root = Path(sys.argv[1])
required = ['Data/config3DModel.yaml', 'Data/sensor_model_providers.json',
            'models', 'vendor/cesiumjs', 'vendor/milsymbol',
            'src/infrastructure/hla/FOM', 'Dependencies/jsbsim/aircraft',
            'Dependencies/jsbsim/engine', 'Dependencies/jsbsim/systems']
for relative in required:
    if not (root / relative).exists():
        raise SystemExit(f'Missing portable resource: {relative}')
if not any((root / 'sensor-plugins').glob('*qttest_mixr_sensor.*')):
    raise SystemExit('MIXR plugin absent')
(root / 'RUNTIME_CAPABILITIES.txt').write_text('''MIXR: AVAILABLE
  Packaged plugin passed native load, ABI, identity and evaluation smoke test.
HLA OpenRTI: DISABLED IN THIS BUILD
HLA Pitch: DISABLED IN THIS BUILD
  SDK-built adapter and user-installed proprietary runtime are required.
ROS2/DDS: DISABLED IN THIS BUILD
  No ROS2 runtime is packaged in this build.
CIGI: NOT IMPLEMENTED
Stone Soup: EXTERNAL RUNTIME REQUIRED
  Requires Python, stonesoup and numpy; no portable Python runtime included.
JSBSim: AVAILABLE
  Built into application; aircraft, engine and systems data packaged.
''', encoding='utf-8')
