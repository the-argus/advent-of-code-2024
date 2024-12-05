const std = @import("std");
const builtin = @import("builtin");
const zcc = @import("compile_commands");

const debug_flags = &[_][]const u8{
    "-g",
    "-std=c++20",
    "-DFMT_EXCEPTIONS=1",
};

const testing_flags = &[_][]const u8{
    "-DFMT_HEADER_ONLY",
    "-I./tests/",
    "-I./include/",
};

const executables = &[_][]const u8{
    "day1/day1.cpp",
    "day2/day2.cpp",
    "day3/day3.cpp",
    "day4/day4.cpp",
};

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const mode = b.standardOptimizeOption(.{});

    var flags = std.ArrayList([]const u8).init(b.allocator);
    defer flags.deinit();
    try flags.appendSlice(debug_flags);
    try flags.appendSlice(testing_flags);

    var targets = std.ArrayList(*std.Build.Step.Compile).init(b.allocator);
    defer targets.deinit();

    const fmt = b.dependency("fmt", .{});
    const fmt_include_path = b.pathJoin(&.{ fmt.builder.install_path, "include" });
    try flags.append(b.fmt("-I{s}", .{fmt_include_path}));

    const flags_owned = flags.toOwnedSlice() catch @panic("OOM");

    for (executables) |source_file| {
        const stem = std.fs.path.stem(source_file);
        var exe = b.addExecutable(.{
            .name = stem,
            .optimize = mode,
            .target = target,
        });
        exe.addCSourceFile(.{
            .file = b.path(source_file),
            .flags = flags_owned,
        });
        exe.linkLibCpp();
        exe.step.dependOn(fmt.builder.getInstallStep());

        const runstep = b.step(b.fmt("run_{s}", .{stem}), "Compile and run all the tests");
        const installstep = b.step(b.fmt("install_{s}", .{stem}), "Install all the tests but don't run them");
        const install_artifact = b.addInstallArtifact(exe, .{});
        installstep.dependOn(&install_artifact.step);

        const run_artifact = b.addRunArtifact(exe);
        runstep.dependOn(&run_artifact.step);

        try targets.append(exe);
    }

    zcc.createStep(b, "cdb", try targets.toOwnedSlice());
}
