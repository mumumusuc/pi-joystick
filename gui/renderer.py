import jinja2
from jinja2 import FileSystemLoader, Environment
from os import path

dirname = path.dirname(path.abspath(__file__))
template_path = path.join(dirname, 'dist')
print('renderer: using template path = ', template_path)
renderer = Environment(loader=FileSystemLoader(template_path))


def render(template, **kw):
    return renderer.get_template(template).render(**kw)
