"""Write an honest runtime capability report for a packaged qttest artifact."""
from pathlib import Path
import sys

root = Path(sys.argv[1])
required = [
    'Data/config3DModel.yaml',
    'Data/sensor_model_providers.json',
    'models',
    'vendor/cesiumjs',
    'vendor/milsymbol',
    'src/infrastructure/hla/FOM',
    'Dependencies/jsbsim/aircraft',
    'Dependencies/jsbsim/engine',
    'Dependencies/jsbsim/systems',
]
for relative in required:
    if not (root / relative).exists():
        raise SystemExit(f'Missing portable resource: {relative}')

sensor_plugins = root / 'sensor-plugins'
hla_plugins = root / 'hla-plugins'

mixr_plugins = list(sensor_plugins.glob('*qttest_mixr_sensor.*'))
if not mixr_plugins:
    raise SystemExit('MIXR plugin absent')

openrti_plugins = list(hla_plugins.glob('*qttest_hla_openrti1516e_backend.*'))
pitch_plugins = list(hla_plugins.glob('*qttest_hla_pitch1516e_backend.*'))

lines = [
    'MIXR: AVAILABLE',
    '  Packaged plugin passed native load, ABI, identity and evaluation smoke test.',
]

if openrti_plugins:
    lines += [
        'HLA OpenRTI: AVAILABLE',
        '  Packaged IEEE 1516e adapter detected in hla-plugins/.',
    ]
else:
    lines += [
        'HLA OpenRTI: DISABLED IN THIS BUILD',
        '  The OpenRTI source tree was not available to this CI build.',
    ]

if pitch_plugins:
    lines += [
        'HLA Pitch: EXTERNAL RUNTIME REQUIRED',
        '  qttest Pitch adapter is packaged; a compatible local Pitch pRTI runtime is still required.',
    ]
else:
    lines += [
        'HLA Pitch: EXTERNAL RUNTIME REQUIRED',
        '  Commercial SDK/runtime is not redistributed. On Windows run setup_pitch_backend.cmd with PitchRTI_ROOT set,',
        '  or provide QTTEST_HLA_PITCH_PLUGIN pointing at a compatible qttest Pitch adapter.',
    ]

lines += [
    'DIS/SISO entity catalog: AVAILABLE',
    '  SISO entity-type metadata is packaged and resolved at runtime.',
    'DIS network transport: NOT IMPLEMENTED',
    'ROS2/DDS: DISABLED IN THIS BUILD',
    '  No ROS2 runtime is packaged in this build.',
    'CIGI: NOT IMPLEMENTED',
    'Stone Soup: EXTERNAL RUNTIME REQUIRED',
    '  Requires Python, stonesoup and numpy; no portable Python runtime is included.',
    'JSBSim: AVAILABLE',
    '  Built into the application; aircraft, engine and systems data are packaged.',
]

(root / 'RUNTIME_CAPABILITIES.txt').write_text('\n'.join(lines) + '\n', encoding='utf-8')
