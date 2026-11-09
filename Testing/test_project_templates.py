import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "Scripts"))

from project_tools import file_util
from project_tools.guid_util import guid_manager
from project_tools.object_info import fl_object
from project_tools.path_util import fl_paths
from project_tools.vs_util import bounds_compiled, fl_solution
from project_tools.xc_util import fl_pbxproj, section_bounds


class ProjectTemplateTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        fl_object.load_cache()
        cls.yin = fl_object.create_from_name("fl.yinfft~")
        cls.fft = fl_object.create_from_name("fl.fft~")
        cls.paths = fl_paths()

    def test_object_templates_and_fallbacks(self):
        for info in (self.yin, self.fft):
            with self.subTest(object=info["object_class"]):
                vs = self.paths.object_vs_template("source", info)
                xc = self.paths.object_xc_template("file_object", info)
                self.assertEqual(Path(vs).name, "YINFFT_source" if info is self.yin else "source")
                self.assertEqual(Path(xc).name, "YINFFT_file_object" if info is self.yin else "file_object")
                self.assertNotIn("_##", file_util.templated_string(vs, info))
                self.assertNotIn("_##", file_util.templated_string(xc, info))

    def test_object_template_name_without_framelib_prefix(self):
        self.assertEqual(self.paths.object_template_name("source", {"object_class_file": "CustomObject"}),
                         "CustomObject_source")

    def test_existing_vs_source_can_be_removed_and_reinserted(self):
        for info in (self.yin, self.fft):
            with self.subTest(object=info["object_class"]):
                project = fl_solution()
                before = project.project.data
                entry = file_util.templated_string(self.paths.object_vs_template("source", info), info)
                self.assertEqual(before.count(entry), 1)
                project.object_project_modify(info, "source", bounds_compiled(), False)
                self.assertNotIn(entry, project.project.data)
                project.object_project_modify(info, "source", bounds_compiled(), True)
                self.assertEqual(project.project.data.count(entry), 1)

    def test_existing_xcode_entries_can_be_removed_and_reinserted(self):
        entries = (
            ("file_object", "PBXBuildFile"),
            ("file_object_for_lib", "PBXBuildFile"),
        )
        for info in (self.yin, self.fft):
            for template, section in entries:
                with self.subTest(object=info["object_class"], template=template):
                    project = fl_pbxproj()
                    entry = file_util.templated_string(self.paths.object_xc_template(template, info), info)
                    self.assertEqual(project.pbxproj.data.count(entry), 1)
                    project.project_modify(info, template, section_bounds(section), False)
                    self.assertNotIn(entry, project.pbxproj.data)
                    project.project_modify(info, template, section_bounds(section), True)
                    self.assertEqual(project.pbxproj.data.count(entry), 1)

    def test_missing_object_configuration_is_reported(self):
        for cache in ({"fl.yinfft~": self.yin.info.copy()}, {}):
            original_xc = guid_manager.xc
            def lookup(manager, section, name):
                if name == self.paths.object_xc_config_name(self.yin):
                    return ""
                return original_xc(manager, section, name)
            with self.subTest(cached=bool(cache)), \
                 patch.object(fl_object, "object_cache", cache.copy()), \
                 patch.object(guid_manager, "xc", autospec=True, side_effect=lookup):
                with self.assertRaisesRegex(ValueError, "Missing Xcode configuration file reference"):
                    fl_object.create_from_name("fl.yinfft~")

    def test_object_xcode_config_exists(self):
        with tempfile.TemporaryDirectory() as directory:
            paths = fl_paths()
            paths.base_dir = directory
            self.assertEqual(paths.object_xc_config_name(self.fft), "Config_FrameLib_FFT_Max.xcconfig")
            self.assertEqual(paths.object_xc_config_name(self.yin), "Config_FrameLib_YINFFT_Max.xcconfig")
            config = Path(directory) / paths.object_xc_config_name(self.fft)
            self.assertFalse(paths.object_xc_config_exists(self.fft))
            config.mkdir()
            self.assertFalse(paths.object_xc_config_exists(self.fft))
            config.rmdir()
            config.touch()
            self.assertTrue(paths.object_xc_config_exists(self.fft))
            self.assertFalse(paths.object_xc_config_exists(self.yin))

    def test_object_specific_templates_use_config_name_from_paths(self):
        names = ("build_config_develop", "build_config_deploy", "build_config_test")
        for name in names:
            with self.subTest(template=name):
                entry = file_util.templated_string(
                    self.paths.xc_template("object_specific_" + name), self.yin)
                guid = self.yin["xc_object_config_guid"]
                self.assertIn(guid + " /* " + self.paths.object_xc_config_name(self.yin) + " */", entry)

    def test_xcode_configurations_share_file_reference(self):
        project = fl_pbxproj().pbxproj.data
        guid = self.yin["xc_object_config_guid"]
        name = self.paths.object_xc_config_name(self.yin)
        entry = (guid + " /* " + name + " */ = {isa = PBXFileReference; "
                 "lastKnownFileType = text.xcconfig; path = " + name + ";")
        self.assertEqual(project.count(entry), 1)
        self.assertNotIn(guid, [self.yin[key] for key in (
            "xc_obj_config_dvmt_guid", "xc_obj_config_dplt_guid", "xc_obj_config_test_guid")])
        self.assertEqual(project.count("baseConfigurationReference = " + guid + " /* " + name + " */;"), 3)

    def test_cached_config_guid_is_refreshed(self):
        cached = self.yin.info.copy()
        cached["xc_object_config_guid"] = "OLD"
        with patch.object(fl_object, "object_cache", {"fl.yinfft~": cached}):
            info = fl_object.create_from_name("fl.yinfft~")
            self.assertEqual(info["xc_object_config_guid"], self.yin["xc_object_config_guid"])

    def test_absent_config_clears_cached_guid(self):
        cached = self.fft.info.copy()
        cached["xc_object_config_guid"] = "OLD"
        with patch.object(fl_object, "object_cache", {"fl.fft~": cached}):
            info = fl_object.create_from_name("fl.fft~")
            self.assertEqual(info["xc_object_config_guid"], "")

    def test_uncached_object_config_guid(self):
        with patch.object(fl_object, "object_cache", {}):
            yin = fl_object.create_from_name("fl.yinfft~")
            fft = fl_object.create_from_name("fl.fft~")
        self.assertEqual(yin["xc_object_config_guid"], self.yin["xc_object_config_guid"])
        self.assertEqual(fft["xc_object_config_guid"], "")

    def test_build_configurations_can_be_removed_and_reinserted(self):
        names = ("build_config_develop", "build_config_deploy", "build_config_test")
        for info, prefix in ((self.yin, "object_specific_"), (self.fft, "")):
            with self.subTest(object=info["object_class"]):
                project = fl_pbxproj()
                entries = [file_util.templated_string(self.paths.xc_template(prefix + name), info)
                           for name in names]
                for entry in entries:
                    self.assertEqual(project.pbxproj.data.count(entry), 1)
                for name in names:
                    project.project_modify_section(info, prefix + name, "XCBuildConfiguration", False)
                for entry in entries:
                    self.assertNotIn(entry, project.pbxproj.data)
                for name in names:
                    project.project_modify_section(info, prefix + name, "XCBuildConfiguration", True)
                for entry in entries:
                    self.assertEqual(project.pbxproj.data.count(entry), 1)

    def test_update_selects_build_configs_from_config_file(self):
        names = ("build_config_develop", "build_config_deploy", "build_config_test")
        for info, prefix in ((self.yin, "object_specific_"), (self.fft, "")):
            for add in (False, True):
                with self.subTest(object=info["object_class"], add=add):
                    project = fl_pbxproj()
                    with patch.object(project, "project_modify_section") as modify_section, \
                         patch.object(project, "project_modify"), \
                         patch.object(project, "project_modify_list"), \
                         patch.object(project, "scheme_modify"), \
                         patch.object(project.pbxproj, "flush"), \
                         patch.object(project.scheme, "flush"):
                        project.update(info, add)
                    configs = [call.args for call in modify_section.call_args_list
                               if call.args[2] == "XCBuildConfiguration"]
                    self.assertEqual(configs, [(info, prefix + name, "XCBuildConfiguration", add)
                                               for name in names])

    def test_templates_work_without_object_cache(self):
        with patch.object(fl_object, "object_cache", {}):
            info = fl_object.create_from_name("fl.yinfft~")
            self.assertEqual(info["object_class_file"], "FrameLib_YINFFT")
            for template in ("file_object", "file_object_for_lib"):
                with self.subTest(template=template):
                    entry = file_util.templated_string(self.paths.object_xc_template(template, info), info)
                    self.assertIn(entry, fl_pbxproj().pbxproj.data)
            for name in ("build_config_develop", "build_config_deploy", "build_config_test"):
                with self.subTest(template=name):
                    entry = file_util.templated_string(
                        self.paths.xc_template("object_specific_" + name), info)
                    self.assertIn(entry, fl_pbxproj().pbxproj.data)
            entry = file_util.templated_string(self.paths.object_vs_template("source", info), info)
            self.assertIn(entry, fl_solution().project.data)


if __name__ == "__main__":
    unittest.main()
