load("@rules_qt//qt:defs.bzl", "qt_cc_binary", "qt_qrc")

filegroup(
    name = "sources",
    srcs = glob(["src/*.cpp"]),
)

filegroup(
    name = "headers",
    srcs = glob(["include/*.h"]),
)

qt_cc_binary(
    name = "raycaster",
    srcs = [":sources"],
    hdrs = [":headers"],
    moc_hdrs = ["include/raycaster_widget.h"],
    deps = [
        ":resources",
        "@qt//:QtCore",
        "@qt//:QtGui",
        "@qt//:QtWidgets",
    ],
)
