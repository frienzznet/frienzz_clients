import json
import subprocess
import sys
import os
import argparse
from typing import Generator
import oyaml
from git import Repo


class DeploySupport:
    def __init__(self, args):
        self.args = args
        try:

            with open(self.args.config, "r", encoding="utf-8") as f:
                self.config = oyaml.load(f, Loader=oyaml.SafeLoader)

        except Exception as e:
            print(f"Error loading config file: {e}")
            sys.exit(1)
        try:
            self.repo = Repo(self.config["repo_path"])
        except Exception as e:
            print(f"Error initializing git repository: {e}")
            sys.exit(1)
        if self.repo.working_tree_dir is None:
            print("Error: The specified path is not a valid git repository.")
            sys.exit(1)
        if self.repo.is_dirty() and not self.args.debug:
            print(
                "Error: The repository has uncommitted changes. Please commit or stash them before deploying."
            )
            sys.exit(1)
        for key in [
            "repo_path",
            "work_dir",
            "build_target",
            "build_image_target_name",
            "dist_target_name",
            "dist_image_target_name",
            "compile_command",
        ]:
            if key not in self.config:
                print(f"Error: '{key}' not found in the configuration file.")
                sys.exit(1)

        git_cmd = self.repo.git
        self.commit_count = int(git_cmd.rev_list("--count", "--first-parent", "HEAD"))
        self.branch_name = self.repo.active_branch.name

    def create_recursive_options(
        self, parent_options_dict: dict, remaining_options: dict
    ):
        """
        Recursively creates a list of all possible combinations of options from a nested dictionary.
        Each key in the dictionary represents an option, and its value is a list of possible values for that option.
        If a value is itself a dictionary, the function will recursively process it to generate combinations.

        Args:
            parent_options_dict (dict): A dictionary containing the parent options.
            remaining_options (dict): A dictionary containing the remaining options to be processed.
        """
        if len(remaining_options) > 1:
            option_iterator = iter(remaining_options)
            this_option = next(option_iterator)
            this_values = remaining_options[this_option]
            deeper_remaining_options = {
                k: v for k, v in remaining_options.items() if k != this_option
            }
            for value in this_values:
                deeper_options = parent_options_dict.copy()
                deeper_options[this_option] = value
                yield from self.create_recursive_options(
                    deeper_options, deeper_remaining_options
                )
        else:
            if len(remaining_options) == 1:
                this_option, this_values = next(iter(remaining_options.items()))
                for value in this_values:
                    deeper_options = parent_options_dict.copy()
                    deeper_options[this_option] = value
                    yield deeper_options
            else:
                yield parent_options_dict

    def replace_placeholders(self, command: str, options: dict) -> str:
        """Replace placeholders in the command string with actual values from the options dictionary."""
        for key, value in options.items():
            command = command.replace(f"%{key}%", str(value))
        return command

    def create_build_variants(self) -> bool:
        """This method would create build variants based on the configuration."""

        print("Creating build variants...")
        for profile, options in self.config.get("profiles", {}).items():
            print(f"Profile: {profile}")
            deeper_options = {"HARDWARE_ID": profile, "PROFILE": profile}
            for branch in options.get("BRANCH_NAME", []):
                # TODO: Add logic to handle different branches if needed
                git_cmd = self.repo.git
                deeper_options["FIRMWARE_VERSION"] = git_cmd.rev_list(
                    "--count", "--first-parent", "HEAD"
                )
                deeper_options["BRANCH_NAME"] = branch
                print(f"Branch: {branch}")
                deeper_remaining_options = {
                    k: v
                    for k, v in options.items()
                    if k not in ["BRANCH_NAME", "chipFamily"]
                }
                for variant in self.create_recursive_options(
                    deeper_options, deeper_remaining_options
                ):
                    print(f"Build variant: {variant}")
                    # continue
                    compile_command = self.config["compile_command"]
                    flags = " ".join(
                        f"-D{key}={value}" for key, value in variant.items()
                    )
                    compile_command = compile_command.replace("%FLAGS%", flags)
                    compile_command = self.replace_placeholders(
                        compile_command, variant
                    )

                    print(f"Compile command: {compile_command}")
                    process = subprocess.Popen(
                        compile_command,
                        shell=True,  # stdout=subprocess.PIPE
                        cwd=self.config["work_dir"],
                    )
                    print(f"Compile command: {compile_command}")
                    process.wait()
                    print(f"process.returncode: {process.returncode}")
                    if process.returncode != 0:
                        print(
                            f"Error: Compilation failed for variant {variant}. Command: {compile_command}"
                        )
                        return False
                    else:
                        build_target = self.replace_placeholders(
                            self.config["build_target"], variant
                        )
                        dist_target_name = self.replace_placeholders(
                            self.config["dist_target_name"], variant
                        )
                        print(f"Build target: {build_target}")
                        print(f"Dist target: {dist_target_name}")
                        if os.path.exists(build_target):
                            os.rename(
                                build_target,
                                os.path.join(self.args.targetdir, dist_target_name),
                            )
                            print(
                                f"Successfully moved {build_target} to {dist_target_name}"
                            )
                        else:
                            print(f"Error: Build target {build_target} does not exist.")
                            return False
                        build_image_target_name = self.replace_placeholders(
                            self.config["build_image_target_name"], variant
                        )
                        dist_image_target_name = self.replace_placeholders(
                            self.config["dist_image_target_name"], variant
                        )
                        print(f"Build image target: {build_image_target_name}")
                        print(f"Dist image target: {dist_image_target_name}")
                        if os.path.exists(build_image_target_name):
                            os.rename(
                                build_image_target_name,
                                os.path.join(
                                    self.args.targetdir, dist_image_target_name
                                ),
                            )
                            print(
                                f"Successfully moved {build_image_target_name} to {dist_image_target_name}"
                            )
                        else:
                            print(
                                f"Error: Build image target {build_image_target_name} does not exist."
                            )
                            return False
        return True

    def create_manifest_dictionary(self) -> bool:
        """Scans the dist directory for available firmware files and returns a list of them."""
        if not os.path.exists(self.args.targetdir):
            print(f"Error: Dist directory {self.args.targetdir} does not exist.")
            return False
        manifest = {}
        for f in os.listdir(self.args.targetdir):
            if os.path.isfile(os.path.join(self.args.targetdir, f)) and f.endswith(
                ".bin"
            ):
                file_name_parts = f.split(".")
                if len(file_name_parts) >= 5:
                    hardware_id = file_name_parts[0]
                    orientation = file_name_parts[1]
                    branch_name = file_name_parts[2]
                    firmware_version = file_name_parts[3]
                    debug = file_name_parts[4]
                    profile = self.config["profiles"].get(hardware_id, {})
                    orientation_text = profile["ORIENTATION"].get(
                        orientation, "Unknown"
                    )
                    branch_text = profile["BRANCH_NAME"].get(branch_name, "Unknown")
                    debug_text = profile["DEBUG"].get(debug, "Unknown")
                    if file_name_parts[5].lower() == "merged":
                        file_id = ".".join(file_name_parts[:6])
                        manifest[file_id] = {
                            "name": hardware_id,
                            "version": ".".join(file_name_parts[1:5]),
                            "innerhtml": f"{hardware_id} - {orientation_text} - {branch_text} - {debug_text} - Version {firmware_version} (Komplett löschen und neu installieren)",
                            "new_install_prompt_erase": False,
                            "builds": [
                                {
                                    "chipFamily": profile.get("chipFamily", "Unknown"),
                                    "parts": [
                                        {
                                            "path": "./" + f,
                                            "offset": 0,
                                        }
                                    ],
                                }
                            ],
                        }
                    else:
                        file_id = ".".join(file_name_parts[:5])
                        manifest[file_id] = {
                            "name": hardware_id,
                            "version": ".".join(file_name_parts[1:4]),
                            "innerhtml": f"{hardware_id} - {orientation_text} - {branch_text} - {debug_text} - Version {firmware_version}",
                            "new_install_prompt_erase": True,
                            "builds": [
                                {
                                    "chipFamily": profile.get("chipFamily", "Unknown"),
                                    "parts": [
                                        {
                                            "path": "./" + f,
                                            "offset": 65536,
                                        }
                                    ],
                                }
                            ],
                        }
        with open(
            os.path.join(self.args.targetdir, "allmanifests.json"),
            "w",
            encoding="utf-8",
        ) as manifest_file:
            json.dump(dict(sorted(manifest.items())), manifest_file, indent=4)
        return True


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Deploy support files to the target device."
    )
    parser.add_argument(
        "--config", required=True, help="Path to the configuration file."
    )
    parser.add_argument(
        "--targetdir", required=True, help="Path to the target directory."
    )
    parser.add_argument(
        "--debug",
        required=False,
        help="Enable debug mode and bypass certain checks.",
        default=False,
        action="store_true",
    )
    parser.add_argument(
        "--manifest",
        required=False,
        help="if set, only  manifest is generated.",
        default=False,
        action="store_true",
    )
    args = parser.parse_args()

    deploy_support = DeploySupport(args)

    # Here you would add the logic to deploy the support files to the target device.
    print(deploy_support.config)
    print(f"Commit count: {deploy_support.commit_count}")
    print(f"Branch name: {deploy_support.branch_name}")
    if args.manifest:
        if not deploy_support.create_manifest_dictionary():
            print("Error: Failed to create manifest dictionary.")
            sys.exit(1)
        sys.exit(0)
    if not deploy_support.create_build_variants():
        print("Error: Failed to create build variants.")
        sys.exit(1)
    if not deploy_support.create_manifest_dictionary():
        print("Error: Failed to create manifest dictionary.")
        sys.exit(1)
