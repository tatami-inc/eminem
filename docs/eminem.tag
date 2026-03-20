<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.12.0">
  <compound kind="file">
    <name>eminem.hpp</name>
    <path>eminem/</path>
    <filename>eminem_8hpp.html</filename>
    <namespace>eminem</namespace>
  </compound>
  <compound kind="file">
    <name>from_gzip.hpp</name>
    <path>eminem/</path>
    <filename>from__gzip_8hpp.html</filename>
    <namespace>eminem</namespace>
  </compound>
  <compound kind="file">
    <name>from_text.hpp</name>
    <path>eminem/</path>
    <filename>from__text_8hpp.html</filename>
    <namespace>eminem</namespace>
  </compound>
  <compound kind="file">
    <name>Parser.hpp</name>
    <path>eminem/</path>
    <filename>Parser_8hpp.html</filename>
    <class kind="struct">eminem::ParserOptions</class>
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
    <templarg>class ReaderPointer_</templarg>
    <templarg>typename Index_</templarg>
    <member kind="function">
      <type></type>
      <name>Parser</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a1af5d6d2c8999f3f176a43c3fb8a4813</anchor>
      <arglist>(ReaderPointer_ input, const ParserOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>const MatrixDetails &amp;</type>
      <name>get_banner</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>afc844f274eb4b187ab297f8cec0a570f</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Index_</type>
      <name>get_nrows</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a0d92e077d1edaf79611f1f3a16128246</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Index_</type>
      <name>get_ncols</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a26fa984587a17b355c4e3d6180dc2354</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>LineIndex</type>
      <name>get_nlines</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>aeaecdb970a67e353dd275dda4acc844c</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scan_preamble</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a1b7906efb9563c3e85fe5c02a97015c0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scan_integer</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>aed6ee2bbb58d8264104a0eae10aa38a0</anchor>
      <arglist>(Store_ store)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scan_real</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>ab67a092b496b8a4a1780133062dc9479</anchor>
      <arglist>(Store_ &amp;&amp;store)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scan_double</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a6ef9728cda0b0cd0b1f0628a3afd121c</anchor>
      <arglist>(Store_ store)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scan_complex</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a52ed0eef29cac0bb01584b277b086817</anchor>
      <arglist>(Store_ store)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scan_pattern</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>af7f7e26cdad47ff654138b00717d0760</anchor>
      <arglist>(Store_ store)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>eminem::ParserOptions</name>
    <filename>structeminem_1_1ParserOptions.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structeminem_1_1ParserOptions.html</anchorfile>
      <anchor>a9583a1a5fd86cfe6b70a66785d8faae4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::size_t</type>
      <name>buffer_size</name>
      <anchorfile>structeminem_1_1ParserOptions.html</anchorfile>
      <anchor>abe638039480119f8ea4b7c0de79d661f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>eminem</name>
    <filename>namespaceeminem.html</filename>
    <class kind="struct">eminem::MatrixDetails</class>
    <class kind="class">eminem::Parser</class>
    <class kind="struct">eminem::ParserOptions</class>
    <member kind="typedef">
      <type>unsigned long long</type>
      <name>LineIndex</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>ae83f2521db54580a7597b3b41a9e30e4</anchor>
      <arglist></arglist>
    </member>
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
    <member kind="function">
      <type>auto</type>
      <name>parse_gzip_file</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>a1b844e52a69a0778626e38614524750a</anchor>
      <arglist>(const char *path, const ParserOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>parse_zlib_buffer</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>a23174c7a760171601aec2fe019eebd46</anchor>
      <arglist>(const unsigned char *buffer, std::size_t len, const ParserOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>parse_some_file</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>a06a1354aad5725023859764dbee88edc</anchor>
      <arglist>(const char *path, const ParserOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>parse_some_buffer</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>a4520bc3100c72d2e866033971fcadb2e</anchor>
      <arglist>(const unsigned char *buffer, std::size_t len, const ParserOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>parse_text_file</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>ac1799b347510527f5dd70b89c949f79d</anchor>
      <arglist>(const char *path, const ParserOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>parse_text_buffer</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>abd3a7597f22e2da803ea3e5167c137ed</anchor>
      <arglist>(const unsigned char *buffer, std::size_t len, const ParserOptions &amp;options)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Parse Matrix Market files in C++</title>
    <filename>index.html</filename>
    <docanchor file="index.html">md__2github_2workspace_2README</docanchor>
  </compound>
</tagfile>
