# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))

# import sphinx_material
import sphinx_material
import os
import sys

# -- Project information -----------------------------------------------------

project = 'ILIAD'
copyright = '2021, Praxinos'
author = 'Praxinos'

# The full version, including alpha/beta/rc tags
release = '0.7'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
sys.path.append(os.path.abspath("./_ext"))

extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.doctest",
    "sphinx.ext.extlinks",
    "sphinx.ext.intersphinx",
    "sphinx.ext.todo",
    "sphinx.ext.mathjax",
    "sphinx.ext.viewcode",
    "myst_parser",
    "sphinx_markdown_tables",
    "sphinx_copybutton",
    "prism"
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
extensions.append("sphinx_material")
html_theme_path = sphinx_material.html_theme_path()
html_context = sphinx_material.get_html_context()
html_theme = 'sphinx_material'

html_theme_options = {
    "nav_title": "ILIAD Developer Documentation",
    "repo_url": "https://github.com/Praxinos/Iliad",
    "repo_type": "github",
    "globaltoc_depth": "-1",
    "globaltoc_collapse": "true",
    "globaltoc_includehidden": "true",
    "theme_color": "2c2c2c",
    "color_primary": "lightblue",
    "color_accent": "blue",
    "master_doc": "",
    "nav_links": [
        {
            "href": "http://praxinos.coop/Documentation/Iliad/Developer/version/v0.6.426/html/index.html",
            "title": "Reference"
        },
        {
            "href": "https://praxinos.coop/",
            "title": "Website"
        }
    ]
}

#You must set html_sidebars in order for the side bar to appear. There are four in the complete set.
html_sidebars = {
    "**": ["logo-text.html", "globaltoc.html", "localtoc.html", "searchbox.html"]
}

html_show_sourcelink = False

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

html_css_files = [
    'prism.css',
]

html_js_files = [
    'prism.js',
]

highlight_language = 'none'

# html_favicon = "images/favicon.ico"

html_use_index = True
html_domain_indices = True
 
todo_include_todos = True