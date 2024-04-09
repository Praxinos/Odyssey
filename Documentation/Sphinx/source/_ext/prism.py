from docutils import nodes
from docutils.nodes import Element, Node
from docutils.parsers.rst import Directive, directives
from docutils.statemachine import StringList
from sphinx.util.docutils import SphinxDirective

def container_wrapper(directive: SphinxDirective, content_node: Node, caption: str) -> nodes.container:  # NOQA
    container_node = nodes.container('', literal_block=False,
                                     classes=[])
    parsed = nodes.Element()
    directive.state.nested_parse(StringList([caption], source=''),
                                 directive.content_offset, parsed)
    if isinstance(parsed[0], nodes.system_message):
        msg = __('Invalid caption: %s' % parsed[0].astext())
        raise ValueError(msg)
    elif isinstance(parsed[0], nodes.Element):
        caption_node = nodes.caption(parsed[0].rawsource, '',
                                     *parsed[0].children)
        caption_node.source = content_node.source
        caption_node.line = content_node.line
        container_node += caption_node
        container_node += content_node
        return container_node
    else:
        raise RuntimeError  # never reached

class Prism(SphinxDirective):

    has_content = True
    required_arguments = 0
    optional_arguments = 1
    final_argument_whitespace = False
    option_spec = {
        'caption': directives.unchanged_required,
    }


    def run(self):
        code = '\n'.join(self.content) #stringify
        if self.arguments:
            code = '\n<pre class="code-block-pre">\n<code class="lang-' + self.arguments[0] + '">\n' + code + '\n</pre>\n</code>'
        else:
            code = '\n<pre class="code-block-pre">\n<code>\n' + code + '\n</pre>\n</code>'

        

        # block: Element = raw_node

        # document = self.state.document
        caption = self.options.get('caption')
        if caption:
            code = '\n<details open class="code-block-details">\n<summary class="code-block-caption">\n' + caption + '\n</summary>' + code +'\n</details>'
        else:
            code = '\n<details open class="code-block-details">\n<summary class="code-block-caption">Code Sample\n</summary>' + code +'\n</details>'

            
            
            
            
            #try:
            #    block = container_wrapper(self, block, caption)
            #except ValueError as exc:
            #    return [document.reporter.warning(exc, line=self.lineno)]

        attributes = {'format': 'html'}
        raw_node = nodes.raw('', code, **attributes)
        (raw_node.source, raw_node.line) = self.state_machine.get_source_and_line(self.lineno)

        return [raw_node]


def setup(app):
    app.add_directive("code-block", Prism, True)

    return {
        'version': '0.1',
        'parallel_read_safe': False,
        'parallel_write_safe': False,
    }
