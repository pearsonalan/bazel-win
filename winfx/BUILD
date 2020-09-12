load("@bazel_tools//src:main/res/win_res.bzl", "windows_resources")

windows_resources(
	name = "winfxres",
	rc_files = ["WinFxApp.rc"],
)

cc_library(
  name = "winfx",
  hdrs = ["winfx.h"],
  srcs = ["winfx.cpp"],
)

cc_binary(
  name = "winfxapp",
  deps = [
  	":winfx",
  	":winfxres"
  ],
  srcs = ["WinFxApp.cpp", "Resource.h"],
  linkopts = ["/SUBSYSTEM:WINDOWS", "user32.lib"],
)
