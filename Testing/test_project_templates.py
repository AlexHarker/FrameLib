import sys
import tempfile
import unittest
import io
import json
from contextlib import redirect_stdout
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "Scripts"))

from project_tools import file_util
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

    def test_cached_config_guid_is_preserved(self):
        cached = self.yin.info.copy()
        cached["xc_object_config_guid"] = "OLD"
        with patch.object(fl_object, "object_cache", {"fl.yinfft~": cached}):
            info = fl_object.create_from_name("fl.yinfft~")
            self.assertEqual(info["xc_object_config_guid"], "OLD")

    def test_absent_config_preserves_cached_guid(self):
        cached = self.fft.info.copy()
        cached["xc_object_config_guid"] = "OLD"
        with patch.object(fl_object, "object_cache", {"fl.fft~": cached}):
            info = fl_object.create_from_name("fl.fft~")
            self.assertEqual(info["xc_object_config_guid"], "OLD")

    def test_uncached_object_config_guid(self):
        with patch.object(fl_object, "object_cache", {}):
            yin = fl_object.create_from_name("fl.yinfft~")
            fft = fl_object.create_from_name("fl.fft~")
        self.assertEqual(yin["xc_object_config_guid"], self.yin["xc_object_config_guid"])
        self.assertEqual(fft["xc_object_config_guid"], "")

    def test_removed_config_uses_cached_guid_for_removal(self):
        project = fl_pbxproj()
        names = ("build_config_develop", "build_config_deploy", "build_config_test")
        cached = self.yin.info.copy()
        with tempfile.TemporaryDirectory() as directory, \
             patch.object(fl_object, "object_cache", {"fl.yinfft~": cached}), \
             patch.object(fl_paths, "object_xc_config_exists", return_value=False), \
             patch.object(fl_paths, "cache_path", return_value=str(Path(directory) / "cache.json")), \
             patch.object(project.pbxproj, "flush"), patch.object(project.scheme, "flush"):
            info = fl_object.create_from_name("fl.yinfft~")
            special = [file_util.templated_string(self.paths.xc_template("object_specific_" + name), info)
                       for name in names]
            default = [file_util.templated_string(self.paths.xc_template(name), info) for name in names]
            output = io.StringIO()
            with redirect_stdout(output):
                project.update(info, False)
                self.assertEqual(info["xc_object_config_guid"], "")
                for entry in special:
                    self.assertNotIn(entry, project.pbxproj.data)
                project.update(info, True)
                for entry in default:
                    self.assertEqual(project.pbxproj.data.count(entry), 1)
                fl_object.save_cache()
                with open(Path(directory) / "cache.json") as cache_file:
                    self.assertEqual(json.load(cache_file)["fl.yinfft~"]["xc_object_config_guid"], "")
                fl_object.object_cache = {}
                fl_object.load_cache()
                info = fl_object.create_from_name("fl.yinfft~")
                project.update(info, False)
                project.update(info, True)
            self.assertNotIn("WARNING - FAILED TO REMOVE", output.getvalue())
            for entry in default:
                self.assertEqual(project.pbxproj.data.count(entry), 1)

    def test_missing_config_file_reference_does_not_invalidate_cache(self):
        project = fl_pbxproj()
        guid = self.yin["xc_object_config_guid"]
        name = self.paths.object_xc_config_name(self.yin)
        reference = (guid + " /* " + name + " */ = {isa = PBXFileReference; "
                     "lastKnownFileType = text.xcconfig; path = " + name + '; sourceTree = "<group>"; };')
        self.assertIn(reference, project.pbxproj.data)
        project.pbxproj.data = project.pbxproj.data.replace(reference, "")
        with patch.object(project.pbxproj, "flush"), patch.object(project.scheme, "flush"), \
             redirect_stdout(io.StringIO()) as output:
            project.update(self.yin, False)
        self.assertIn("WARNING - FAILED TO REMOVE", output.getvalue())
        for name in ("build_config_develop", "build_config_deploy", "build_config_test"):
            entry = file_util.templated_string(self.paths.xc_template("object_specific_" + name), self.yin)
            self.assertNotIn(entry, project.pbxproj.data)
        self.assertEqual(self.yin["xc_object_config_guid"], guid)

    def test_removal_of_unused_config_reference(self):
        project = fl_pbxproj()
        with patch.object(fl_object, "object_cache", {"fl.yinfft~": self.yin.info.copy()}):
            info = fl_object.create_from_name("fl.yinfft~")
        guid = info["xc_object_config_guid"]
        reference = file_util.templated_string(self.paths.xc_template("fileref_object_config"), info)
        group = file_util.templated_string(self.paths.xc_template("group_item_object_config"), info)
        self.assertIn(reference, project.pbxproj.data)
        self.assertIn(group, project.pbxproj.data)
        with patch.object(project.pbxproj, "flush"), patch.object(project.scheme, "flush"), \
             redirect_stdout(io.StringIO()) as output:
            project.update(info, False)
        self.assertNotIn(reference, project.pbxproj.data)
        self.assertNotIn(group, project.pbxproj.data)
        self.assertEqual(info["xc_object_config_guid"], guid)
        self.assertNotIn("WARNING - FAILED TO REMOVE", output.getvalue())

    def test_removal_preserves_shared_config_reference(self):
        project = fl_pbxproj()
        with patch.object(fl_object, "object_cache", {"fl.yinfft~": self.yin.info.copy()}):
            info = fl_object.create_from_name("fl.yinfft~")
        reference = file_util.templated_string(self.paths.xc_template("fileref_object_config"), info)
        group = file_util.templated_string(self.paths.xc_template("group_item_object_config"), info)
        shared = "\t\t\tbaseConfigurationReference = " + info["xc_object_config_guid"] + " /* shared */;\n"
        marker = "/* Begin XCBuildConfiguration section */\n"
        project.pbxproj.data = project.pbxproj.data.replace(marker, marker + shared)
        with patch.object(project.pbxproj, "flush"), patch.object(project.scheme, "flush"):
            project.update(info, False)
        self.assertIn(reference, project.pbxproj.data)
        self.assertIn(group, project.pbxproj.data)

    def test_rebuild_reuses_removed_config_guid(self):
        project = fl_pbxproj()
        with patch.object(fl_object, "object_cache", {"fl.yinfft~": self.yin.info.copy()}):
            info = fl_object.create_from_name("fl.yinfft~")
        guid = info["xc_object_config_guid"]
        reference = file_util.templated_string(self.paths.xc_template("fileref_object_config"), info)
        group = file_util.templated_string(self.paths.xc_template("group_item_object_config"), info)
        with patch("project_tools.guid_util.guid_manager") as manager, \
             patch.object(project.pbxproj, "flush"), patch.object(project.scheme, "flush"), \
             redirect_stdout(io.StringIO()) as output:
            manager.return_value.xc.return_value = ""
            project.update(info, False)
            project.update(info, True)
        self.assertEqual(info["xc_object_config_guid"], guid)
        manager.return_value.create_xc.assert_not_called()
        self.assertEqual(project.pbxproj.data.count(reference), 1)
        self.assertEqual(project.pbxproj.data.count(group), 1)
        self.assertNotIn("WARNING - FAILED TO REMOVE", output.getvalue())

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

    def test_update_selects_build_configs(self):
        names = ("build_config_develop", "build_config_deploy", "build_config_test")
        for info in (self.yin, self.fft):
            for add in (False, True):
                for config_exists in (False, True):
                    with self.subTest(object=info["object_class"], add=add, config_exists=config_exists):
                        with patch.object(fl_object, "object_cache", {info["max_class_name"]: info.info.copy()}):
                            current = fl_object.create_from_name(info["max_class_name"])
                        project = fl_pbxproj()
                        with patch.object(fl_paths, "object_xc_config_exists", return_value=config_exists), \
                             patch("project_tools.guid_util.guid_manager") as manager, \
                             patch.object(project, "project_modify_section") as modify_section, \
                             patch.object(project, "project_modify"), \
                             patch.object(project, "project_modify_list"), \
                             patch.object(project, "project_add_object_config") as add_object_config, \
                             patch.object(project, "scheme_modify"), \
                             patch.object(project.pbxproj, "flush"), \
                             patch.object(project.scheme, "flush"):
                            previous_guid = current["xc_object_config_guid"]
                            generated_guid = "1234567890ABCDEF12345678"
                            manager.return_value.xc.return_value = previous_guid
                            manager.return_value.create_xc.return_value = generated_guid
                            prefix = "object_specific_" if (config_exists if add else previous_guid) else ""
                            project.update(current, add)
                        configs = [call.args for call in modify_section.call_args_list
                                   if call.args[2] == "XCBuildConfiguration"]
                        self.assertEqual(configs, [(current, prefix + name, "XCBuildConfiguration", add)
                                                   for name in names])
                        expected_guid = previous_guid
                        if add and config_exists and not previous_guid:
                            expected_guid = generated_guid
                            modify_section.assert_any_call(current, "fileref_object_config", "PBXFileReference", True)
                            add_object_config.assert_called_once_with(current)
                        elif not add and not config_exists:
                            expected_guid = ""
                        self.assertEqual(current["xc_object_config_guid"], expected_guid)

    def test_object_config_group_position(self):
        project = fl_pbxproj()
        info = self.fft.info.copy()
        info["xc_object_config_guid"] = "1234567890ABCDEF12345678"
        project.project_add_object_config(info)
        shared = "/* Config_FrameLib_Objects.xcconfig */,\n"
        added = file_util.templated_string(self.paths.xc_template("group_item_object_config"), info)
        self.assertIn(shared + added, project.pbxproj.data)

    def test_object_config_group_position_without_objects_config(self):
        project = fl_pbxproj()
        info = self.fft.info.copy()
        info["xc_object_config_guid"] = "1234567890ABCDEF12345678"
        shared = "\t\t\t\tAABCE1000000000000000001 /* Config_FrameLib_Objects.xcconfig */,\n"
        self.assertIn(shared, project.pbxproj.data)
        project.pbxproj.data = project.pbxproj.data.replace(shared, "")
        project.project_add_object_config(info)
        added = file_util.templated_string(self.paths.xc_template("group_item_object_config"), info)
        self.assertIn("/* Config_FrameLib.xcconfig */,\n" + added, project.pbxproj.data)

    def test_object_config_group_warns_without_shared_config(self):
        project = fl_pbxproj()
        info = self.fft.info.copy()
        info["xc_object_config_guid"] = "1234567890ABCDEF12345678"
        project.pbxproj.data = project.pbxproj.data.replace(
            "/* Config_FrameLib_Objects.xcconfig */,", "/* missing objects config */,")
        project.pbxproj.data = project.pbxproj.data.replace(
            "/* Config_FrameLib.xcconfig */,", "/* missing main config */,")
        with redirect_stdout(io.StringIO()) as output:
            project.project_add_object_config(info)
        self.assertIn("WARNING - Missing shared configuration entry", output.getvalue())
        added = file_util.templated_string(self.paths.xc_template("group_item_object_config"), info)
        self.assertIn(added, project.pbxproj.data)

    def test_failed_flush_preserves_cached_guid(self):
        project = fl_pbxproj()
        with patch.object(fl_object, "object_cache", {"fl.yinfft~": self.yin.info.copy()}):
            info = fl_object.create_from_name("fl.yinfft~")
        guid = info["xc_object_config_guid"]
        with patch.object(fl_paths, "object_xc_config_exists", return_value=False), \
             patch.object(project.pbxproj, "flush", side_effect=OSError("write failed")):
            with self.assertRaisesRegex(OSError, "write failed"):
                project.update(info, False)
        self.assertEqual(info["xc_object_config_guid"], guid)

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
