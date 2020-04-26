def Settings( **kwargs ):
    return {
            'flags': [
                '-x', 'c++',
                '-Wall', '-Wextra', '-Werror',
                '-I', '.',
                '-I', './bazel-knapsacksolver/external/json/single_include/',
                '-I', './bazel-knapsacksolver/external/googletest/googletest-release-1.8.0/googletest/include/',
                '-I', './bazel-knapsacksolver/external/optimizationtools/',
                '-I', './bazel-knapsacksolver/external/pybind11/include/',
                ],
            }

