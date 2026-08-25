from pathlib import Path
import re
import unittest


REPOSITORY_ROOT = Path(__file__).resolve().parents[3]
INTERFACE_DIRECTORY = (
    REPOSITORY_ROOT / "integrations" / "ros2" / "qttest_interfaces" / "msg"
)
NATIVE_BACKEND = (
    REPOSITORY_ROOT
    / "src"
    / "infrastructure"
    / "Ros2TelemetryBackendNative.cpp"
)


def message_fields(path: Path) -> list[str]:
    fields = []
    for raw_line in path.read_text(encoding="utf-8").splitlines():
        line = raw_line.split("#", 1)[0].strip()
        if not line:
            continue
        fields.append(line.split()[-1])
    return fields


class NativeRos2ContractTest(unittest.TestCase):
    def test_all_interface_fields_are_mapped_by_native_backend(self) -> None:
        source = NATIVE_BACKEND.read_text(encoding="utf-8")
        for message_name in ("EngineTelemetry.msg", "KinematicsTelemetry.msg"):
            for field in message_fields(INTERFACE_DIRECTORY / message_name):
                assignment = rf"message\.{re.escape(field)}\s*="
                collection_write = rf"message\.{re.escape(field)}\.(push_back|reserve)"
                self.assertTrue(
                    re.search(assignment, source)
                    or re.search(collection_write, source),
                    f"ROS 2 field is not mapped: {message_name}:{field}",
                )

    def test_qttest_ros2_publication_path_contains_no_websocket(self) -> None:
        files = [
            NATIVE_BACKEND,
            REPOSITORY_ROOT
            / "src"
            / "infrastructure"
            / "Ros2TelemetryPublisher.cpp",
        ]
        for path in files:
            self.assertNotIn("websocket", path.read_text(encoding="utf-8").lower())


if __name__ == "__main__":
    unittest.main()
