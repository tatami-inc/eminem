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
    <class kind="struct">eminem::ParseGzipFileOptions</class>
    <class kind="struct">eminem::ParseZlibBufferOptions</class>
    <class kind="struct">eminem::ParseSomeFileOptions</class>
    <class kind="struct">eminem::ParseSomeBufferOptions</class>
    <namespace>eminem</namespace>
  </compound>
  <compound kind="file">
    <name>from_text.hpp</name>
    <path>eminem/</path>
    <filename>from__text_8hpp.html</filename>
    <class kind="struct">eminem::ParseTextFileOptions</class>
    <class kind="struct">eminem::ParseTextBufferOptions</class>
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
  <compound kind="struct">
    <name>eminem::ParseGzipFileOptions</name>
    <filename>structeminem_1_1ParseGzipFileOptions.html</filename>
    <member kind="variable">
      <type>std::size_t</type>
      <name>buffer_size</name>
      <anchorfile>structeminem_1_1ParseGzipFileOptions.html</anchorfile>
      <anchor>ab19247ab480aefa44c72df865f69f75b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>parallel</name>
      <anchorfile>structeminem_1_1ParseGzipFileOptions.html</anchorfile>
      <anchor>a2aa94d23c5c6bf47ada89b9ef7aefc22</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>eminem::Parser</name>
    <filename>classeminem_1_1Parser.html</filename>
    <templarg>class Input_</templarg>
    <member kind="function">
      <type></type>
      <name>Parser</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a52c38157836f359c75fa89e35265ed8a</anchor>
      <arglist>(std::unique_ptr&lt; Input_ &gt; input)</arglist>
    </member>
    <member kind="function">
      <type>const MatrixDetails &amp;</type>
      <name>get_banner</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a4326ef24eddeca2ff45e00bbfba9f851</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Index</type>
      <name>get_nrows</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>aab9cec4211cc545e2c8550267fa648f1</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Index</type>
      <name>get_ncols</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a1d5fe77163613f3538e0466fdf38f8a2</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Index</type>
      <name>get_nlines</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>aac1969c401b04521698edd6b75d03c5a</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scan_preamble</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a91d2ea36cd1962b19675eac852b77d54</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scan_integer</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a3e5320df82912206164d5fcafb7f1f4e</anchor>
      <arglist>(Store_ store)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scan_real</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a90d0c99c93cd42aad1d455693cacbde4</anchor>
      <arglist>(Store_ &amp;&amp;store)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scan_double</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a9136c73b661df9587a36e79d52b995eb</anchor>
      <arglist>(Store_ store)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scan_complex</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a67a0424acaeb50b22474dfd77010c2be</anchor>
      <arglist>(Store_ store)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scan_pattern</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a65285363373e976d790a2ff764621f86</anchor>
      <arglist>(Store_ store)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>eminem::ParseSomeBufferOptions</name>
    <filename>structeminem_1_1ParseSomeBufferOptions.html</filename>
    <member kind="variable">
      <type>std::size_t</type>
      <name>buffer_size</name>
      <anchorfile>structeminem_1_1ParseSomeBufferOptions.html</anchorfile>
      <anchor>a75882536a96f5c821b1861adb4cc5d5a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>parallel</name>
      <anchorfile>structeminem_1_1ParseSomeBufferOptions.html</anchorfile>
      <anchor>a1afca52341b04111dd53609d04a81e2d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>eminem::ParseSomeFileOptions</name>
    <filename>structeminem_1_1ParseSomeFileOptions.html</filename>
    <member kind="variable">
      <type>std::size_t</type>
      <name>buffer_size</name>
      <anchorfile>structeminem_1_1ParseSomeFileOptions.html</anchorfile>
      <anchor>aacb59a4c82085bf81406660a18deba31</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>parallel</name>
      <anchorfile>structeminem_1_1ParseSomeFileOptions.html</anchorfile>
      <anchor>a2fe6b606009ea7e22ee3b8487b566044</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>eminem::ParseTextBufferOptions</name>
    <filename>structeminem_1_1ParseTextBufferOptions.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>parallel</name>
      <anchorfile>structeminem_1_1ParseTextBufferOptions.html</anchorfile>
      <anchor>a4561d8fe6dc7e046a02f9a1107a7ba82</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>eminem::ParseTextFileOptions</name>
    <filename>structeminem_1_1ParseTextFileOptions.html</filename>
    <member kind="variable">
      <type>std::size_t</type>
      <name>buffer_size</name>
      <anchorfile>structeminem_1_1ParseTextFileOptions.html</anchorfile>
      <anchor>af5501621fa557e59b430f46f59186eb0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>parallel</name>
      <anchorfile>structeminem_1_1ParseTextFileOptions.html</anchorfile>
      <anchor>a04c9f4d93b2f23818d54e56eae12f1f5</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>eminem::ParseZlibBufferOptions</name>
    <filename>structeminem_1_1ParseZlibBufferOptions.html</filename>
    <member kind="variable">
      <type>std::size_t</type>
      <name>buffer_size</name>
      <anchorfile>structeminem_1_1ParseZlibBufferOptions.html</anchorfile>
      <anchor>a1e5d98e401e06702b79a140aab928bbf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>parallel</name>
      <anchorfile>structeminem_1_1ParseZlibBufferOptions.html</anchorfile>
      <anchor>adc78eda83dee03716a97e436ff585e71</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>mode</name>
      <anchorfile>structeminem_1_1ParseZlibBufferOptions.html</anchorfile>
      <anchor>a5df289411df6f70d84ef3a9e47e2e2bc</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>eminem</name>
    <filename>namespaceeminem.html</filename>
    <class kind="struct">eminem::MatrixDetails</class>
    <class kind="struct">eminem::ParseGzipFileOptions</class>
    <class kind="class">eminem::Parser</class>
    <class kind="struct">eminem::ParseSomeBufferOptions</class>
    <class kind="struct">eminem::ParseSomeFileOptions</class>
    <class kind="struct">eminem::ParseTextBufferOptions</class>
    <class kind="struct">eminem::ParseTextFileOptions</class>
    <class kind="struct">eminem::ParseZlibBufferOptions</class>
    <member kind="typedef">
      <type>unsigned long long</type>
      <name>Index</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>a1b89d92edc9a5418b920487f16b2d11b</anchor>
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
      <type>Parser&lt; byteme::PerByteInterface&lt; char &gt; &gt;</type>
      <name>parse_gzip_file</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>ac799ce84829321eab0017b0aafda6095</anchor>
      <arglist>(const char *path, const ParseGzipFileOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Parser&lt; byteme::PerByteInterface&lt; char &gt; &gt;</type>
      <name>parse_zlib_buffer</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>a67592287d45d69690fb1f92dc33fbe00</anchor>
      <arglist>(const unsigned char *buffer, std::size_t len, const ParseZlibBufferOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Parser&lt; byteme::PerByteInterface&lt; char &gt; &gt;</type>
      <name>parse_some_file</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>ac7256bf312f19af11148c03bdb4b91b1</anchor>
      <arglist>(const char *path, const ParseSomeFileOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Parser&lt; byteme::PerByteInterface&lt; char &gt; &gt;</type>
      <name>parse_some_buffer</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>a8f2c47f36720d41fa54eb450135e577b</anchor>
      <arglist>(const unsigned char *buffer, std::size_t len, const ParseSomeBufferOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Parser&lt; byteme::PerByteInterface&lt; char &gt; &gt;</type>
      <name>parse_text_file</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>a68fcb4f6d63322b9107542ded5f7753f</anchor>
      <arglist>(const char *path, const ParseTextFileOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Parser&lt; byteme::PerByteInterface&lt; char &gt; &gt;</type>
      <name>parse_text_buffer</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>af22bebcce31f5162d7fbaf1e4545e86b</anchor>
      <arglist>(const unsigned char *buffer, std::size_t len, const ParseTextBufferOptions &amp;options)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Parse Matrix Market files in C++</title>
    <filename>index.html</filename>
    <docanchor file="index.html">md__2github_2workspace_2README</docanchor>
  </compound>
</tagfile>
