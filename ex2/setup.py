from setuptools import setup, find_packages, Extension

setup(
    name= "mykmeanssp",
    version="0.1.0",
    author="Keren Rozen",
    description="C-API for kmeans",
    install_requires=['invoke'],
    packages=find_packages(),
    ext_modules=[
        Extension(
            'mykmeanssp',
            ['kmeans.c'],
        ),
    ]
)