<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.9.8">
  <compound kind="file">
    <name>eminem.hpp</name>
    <path>eminem/</path>
    <filename>eminem_8hpp.html</filename>
    <namespace>eminem</namespace>
  </compound>
  <compound kind="file">
    <name>from_text.hpp</name>
    <path>eminem/</path>
    <filename>from__text_8hpp.html</filename>
    <class kind="struct">eminem::TextFileParser</class>
    <class kind="struct">eminem::TextBufferParser</class>
    <namespace>eminem</namespace>
  </compound>
  <compound kind="file">
    <name>Parser.hpp</name>
    <path>eminem/</path>
    <filename>Parser_8hpp.html</filename>
    <class kind="class">eminem::Parser</class>
    <namespace>eminem</namespace>
  </compound>
  <compound kind="file">
    <name>utils.hpp</name>
    <path>eminem/</path>
    <filename>utils_8hpp.html</filename>
    <class kind="struct">eminem::MatrixDetails</class>
    <namespace>eminem</namespace>
  </compound>
  <compound kind="struct">
    <name>eminem::GzipFileParser</name>
    <filename>structeminem_1_1GzipFileParser.html</filename>
    <templarg>bool parallel_</templarg>
    <base>eminem::Parser</base>
    <member kind="function">
      <type></type>
      <name>GzipFileParser</name>
      <anchorfile>structeminem_1_1GzipFileParser.html</anchorfile>
      <anchor>a0f08b1f954fb18da51ff7bd07e792921</anchor>
      <arglist>(const char *path, size_t buffer_size=65536)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>eminem::MatrixDetails</name>
    <filename>structeminem_1_1MatrixDetails.html</filename>
    <member kind="variable">
      <type>Object</type>
      <name>object</name>
      <anchorfile>structeminem_1_1MatrixDetails.html</anchorfile>
      <anchor>adbae3a51eba68d76b79306c070e0445d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Format</type>
      <name>format</name>
      <anchorfile>structeminem_1_1MatrixDetails.html</anchorfile>
      <anchor>ab9c95df46dcf80cc827e1ac83bc2e553</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Field</type>
      <name>field</name>
      <anchorfile>structeminem_1_1MatrixDetails.html</anchorfile>
      <anchor>ae18841f01812b0fdfdaf874ae066d719</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Symmetry</type>
      <name>symmetry</name>
      <anchorfile>structeminem_1_1MatrixDetails.html</anchorfile>
      <anchor>a55f737773c4168999e7f9ae118db7cce</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>eminem::Parser</name>
    <filename>classeminem_1_1Parser.html</filename>
    <templarg>bool parallel_</templarg>
    <templarg>class Pointer_</templarg>
    <member kind="function">
      <type></type>
      <name>Parser</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>aac50ba8e6cf38ef0bf917da3c3c1b38b</anchor>
      <arglist>(Pointer_ r)</arglist>
    </member>
    <member kind="function">
      <type>const MatrixDetails &amp;</type>
      <name>get_banner</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>ae328fb6bf0a7c14d94613be7fdb29b68</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>get_nrows</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>af542456a3d6f975771d83d0ca3b10743</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>get_ncols</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a96b8735665f05ad5d62ef63d9d03015f</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>get_nlines</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>af7b5baa96fb3a6daef738e7e838b4354</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scan_preamble</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a7f3b83f6181970fcc27cd4f62701892a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scan_integer</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a09a4fc024e115cec86bedc8af400914c</anchor>
      <arglist>(Store_ &amp;&amp;store)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scan_real</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a9b1a79df5d518569e49d2ef364416414</anchor>
      <arglist>(Store_ &amp;&amp;store)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scan_double</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a56fc759e2286411d823c30fe3f9cd7e3</anchor>
      <arglist>(Store_ &amp;&amp;store)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scan_complex</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a267e51f35b56bed1c16d84e6d040272d</anchor>
      <arglist>(Store_ &amp;&amp;store)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scan_pattern</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a686e70e6d1bd705fc1548b9253f4923e</anchor>
      <arglist>(Store_ &amp;&amp;store)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>eminem::SomeBufferParser</name>
    <filename>structeminem_1_1SomeBufferParser.html</filename>
    <templarg>bool parallel_</templarg>
    <base>eminem::Parser</base>
    <member kind="function">
      <type></type>
      <name>SomeBufferParser</name>
      <anchorfile>structeminem_1_1SomeBufferParser.html</anchorfile>
      <anchor>ae29ef7ae4e9d8a61ceb3864a0e554827</anchor>
      <arglist>(const unsigned char *buffer, size_t len, size_t buffer_size=65536)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>eminem::SomeFileParser</name>
    <filename>structeminem_1_1SomeFileParser.html</filename>
    <templarg>bool parallel_</templarg>
    <base>eminem::Parser</base>
    <member kind="function">
      <type></type>
      <name>SomeFileParser</name>
      <anchorfile>structeminem_1_1SomeFileParser.html</anchorfile>
      <anchor>a88b2dab114576afabfb3da7b2064bbb1</anchor>
      <arglist>(const char *path, size_t buffer_size=65536)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>eminem::TextBufferParser</name>
    <filename>structeminem_1_1TextBufferParser.html</filename>
    <templarg>bool parallel_</templarg>
    <base>eminem::Parser</base>
    <member kind="function">
      <type></type>
      <name>TextBufferParser</name>
      <anchorfile>structeminem_1_1TextBufferParser.html</anchorfile>
      <anchor>a4b6ee958967c782770d11cc016fff8f6</anchor>
      <arglist>(const unsigned char *buffer, size_t len)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TextBufferParser</name>
      <anchorfile>structeminem_1_1TextBufferParser.html</anchorfile>
      <anchor>a4ed6a3a45f609796eab4c977bbaf62fd</anchor>
      <arglist>(const char *buffer, size_t len)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>eminem::TextFileParser</name>
    <filename>structeminem_1_1TextFileParser.html</filename>
    <templarg>bool parallel_</templarg>
    <base>eminem::Parser</base>
    <member kind="function">
      <type></type>
      <name>TextFileParser</name>
      <anchorfile>structeminem_1_1TextFileParser.html</anchorfile>
      <anchor>a6b63c87bfd5035a635a806d5510b7e1c</anchor>
      <arglist>(const char *path, size_t chunk_size=65536)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>eminem::ZlibBufferParser</name>
    <filename>structeminem_1_1ZlibBufferParser.html</filename>
    <templarg>bool parallel_</templarg>
    <base>eminem::Parser</base>
    <member kind="function">
      <type></type>
      <name>ZlibBufferParser</name>
      <anchorfile>structeminem_1_1ZlibBufferParser.html</anchorfile>
      <anchor>ad6e5ddabbe74915a17ed21f91b5d2ad3</anchor>
      <arglist>(const unsigned char *buffer, size_t len, int mode=3, size_t buffer_size=65536)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>eminem</name>
    <filename>namespaceeminem.html</filename>
    <class kind="struct">eminem::GzipFileParser</class>
    <class kind="struct">eminem::MatrixDetails</class>
    <class kind="class">eminem::Parser</class>
    <class kind="struct">eminem::SomeBufferParser</class>
    <class kind="struct">eminem::SomeFileParser</class>
    <class kind="struct">eminem::TextBufferParser</class>
    <class kind="struct">eminem::TextFileParser</class>
    <class kind="struct">eminem::ZlibBufferParser</class>
    <member kind="enumeration">
      <type></type>
      <name>Object</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>afc96f545c5ba3837ed9ab101638f7bdb</anchor>
      <arglist></arglist>
      <enumvalue file="namespaceeminem.html" anchor="afc96f545c5ba3837ed9ab101638f7bdba38ef5fe49a654647519ee8c498c49664">MATRIX</enumvalue>
      <enumvalue file="namespaceeminem.html" anchor="afc96f545c5ba3837ed9ab101638f7bdba87752381b583740610f1dfeb07fdad7e">VECTOR</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>Format</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>a69bd207796eeac4f00392518edfdf001</anchor>
      <arglist></arglist>
      <enumvalue file="namespaceeminem.html" anchor="a69bd207796eeac4f00392518edfdf001acc67df6b110431e6059d25c34d184ede">COORDINATE</enumvalue>
      <enumvalue file="namespaceeminem.html" anchor="a69bd207796eeac4f00392518edfdf001acb4fb1757fb37c43cded35d3eb857c43">ARRAY</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>Field</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>a68f327624182e772e6377016bd8914d2</anchor>
      <arglist></arglist>
      <enumvalue file="namespaceeminem.html" anchor="a68f327624182e772e6377016bd8914d2a8cf125b0e31559ba75a9d9b4f818a554">REAL</enumvalue>
      <enumvalue file="namespaceeminem.html" anchor="a68f327624182e772e6377016bd8914d2afd3e4ece78a7d422280d5ed379482229">DOUBLE</enumvalue>
      <enumvalue file="namespaceeminem.html" anchor="a68f327624182e772e6377016bd8914d2a921a0157a6e61eebbaa0f713fdfbb0f7">COMPLEX</enumvalue>
      <enumvalue file="namespaceeminem.html" anchor="a68f327624182e772e6377016bd8914d2a5d5cd46919fa987731fb2edefe0f2a0c">INTEGER</enumvalue>
      <enumvalue file="namespaceeminem.html" anchor="a68f327624182e772e6377016bd8914d2a04ab50cd8ffc1031a09ac85aa6c5f76a">PATTERN</enumvalue>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>Symmetry</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>a3222df29bae0a3f6387230b58f36e920</anchor>
      <arglist></arglist>
      <enumvalue file="namespaceeminem.html" anchor="a3222df29bae0a3f6387230b58f36e920ab61773b9b3968a9988d765d728985862">GENERAL</enumvalue>
      <enumvalue file="namespaceeminem.html" anchor="a3222df29bae0a3f6387230b58f36e920a161b3d9016563aba9ac190fc02ada9bb">SYMMETRIC</enumvalue>
      <enumvalue file="namespaceeminem.html" anchor="a3222df29bae0a3f6387230b58f36e920adc8503e735d82a5692e41072ee385da0">SKEW_SYMMETRIC</enumvalue>
      <enumvalue file="namespaceeminem.html" anchor="a3222df29bae0a3f6387230b58f36e920a7ef6fdd31540cc6c5a031f31c22f2bde">HERMITIAN</enumvalue>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Parse Matrix Market files in C++</title>
    <filename>index.html</filename>
    <docanchor file="index.html" title="Parse Matrix Market files in C++">md__2github_2workspace_2README</docanchor>
  </compound>
</tagfile>
