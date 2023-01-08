def Settings(**kwargs):
    return {
            'flags': [
                '-x', 'c++',
                '-Wall', '-Wextra', '-Werror',
                '-I', '.',

                '-I', './bazel-knapsacksolver/external/'
                'json/single_include/',

                '-I', './bazel-knapsacksolver/external/'
                'googletest/googletest/include/',

                '-I', './bazel-knapsacksolver/external/'
                'boost/',

                # optimizationtools
                '-I', './bazel-knapsacksolver/external/'
                # '-I', './../'
                'optimizationtools/',

                # Python
                '-I', './bazel-knapsacksolver/external/pybind11/include/',
                '-I', '/usr/include/python2.7/',

                ],
            }
