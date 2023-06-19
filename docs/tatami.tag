<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.9.5">
  <compound kind="file">
    <name>eminem.hpp</name>
    <path>/github/workspace/include/eminem/</path>
    <filename>eminem_8hpp.html</filename>
    <namespace>eminem</namespace>
  </compound>
  <compound kind="file">
    <name>Parser.hpp</name>
    <path>/github/workspace/include/eminem/</path>
    <filename>Parser_8hpp.html</filename>
    <class kind="class">eminem::Parser</class>
    <namespace>eminem</namespace>
  </compound>
  <compound kind="file">
    <name>utils.hpp</name>
    <path>/github/workspace/include/eminem/</path>
    <filename>utils_8hpp.html</filename>
    <class kind="struct">eminem::MatrixDetails</class>
    <namespace>eminem</namespace>
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
    <member kind="function">
      <type></type>
      <name>Parser</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>aa7ec9979303690e07fb426d141b18cd3</anchor>
      <arglist>(byteme::Reader &amp;r)</arglist>
    </member>
    <member kind="function">
      <type>const MatrixDetails &amp;</type>
      <name>get_banner</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>ad08fffd43c2ec0e457a990d229179f39</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>get_nrows</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>af229fc293af0a5ecc666bfd1fa291d58</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>get_ncols</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a06af0c623a88672c2de19c706d50ce7b</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>get_nlines</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a7429a2f280e5387df208260fa5149e7c</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scan_preamble</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>ae3bda1dea3bfe1d974662c4bbc612eb5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scan_integer</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a0c4f34306f89951fd50bac30dad805d7</anchor>
      <arglist>(Store_ &amp;&amp;store)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scan_real</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>aa7bc49b85f09b580ac26c18409c7173e</anchor>
      <arglist>(Store_ &amp;&amp;store)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scan_double</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>ad1755b5584420b1c4cbc1be354590fe9</anchor>
      <arglist>(Store_ &amp;&amp;store)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scan_complex</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>ac0160741f5132c3e5d3b3982e08e7a00</anchor>
      <arglist>(Store_ &amp;&amp;store)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scan_pattern</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>ad9f26d7647241d98034efed26a0e6ccc</anchor>
      <arglist>(Store_ &amp;&amp;store)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>eminem</name>
    <filename>namespaceeminem.html</filename>
    <class kind="struct">eminem::MatrixDetails</class>
    <class kind="class">eminem::Parser</class>
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
    <title>Simple Matrix Market reader</title>
    <filename>index.html</filename>
    <docanchor file="index.html">md__github_workspace_README</docanchor>
  </compound>
</tagfile>
