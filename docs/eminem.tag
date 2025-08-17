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
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structeminem_1_1ParseGzipFileOptions.html</anchorfile>
      <anchor>ac314e31ba48d5b343fec3e5de3057e69</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::size_t</type>
      <name>block_size</name>
      <anchorfile>structeminem_1_1ParseGzipFileOptions.html</anchorfile>
      <anchor>aa96e64cd7f20c7621976a2667fb34612</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>eminem::Parser</name>
    <filename>classeminem_1_1Parser.html</filename>
    <templarg>class Input_</templarg>
    <templarg>typename Index_</templarg>
    <member kind="function">
      <type></type>
      <name>Parser</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a449fdbbe6fccfaedd23993e2d61fd741</anchor>
      <arglist>(std::unique_ptr&lt; Input_ &gt; input, const ParserOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>const MatrixDetails &amp;</type>
      <name>get_banner</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a4073ef8fc24d3403c1ac655b82f99179</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Index_</type>
      <name>get_nrows</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a0a82592efbbe5cfc528c8618c787f0af</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Index_</type>
      <name>get_ncols</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>ad0245f01da0ffdac6511315c94da3e1e</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>LineIndex</type>
      <name>get_nlines</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a36dbe8d651129ddce69b9db0e594f8a1</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scan_preamble</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a04f29bcd5780761aff7ab09b7a8325ea</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scan_integer</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>af565251e8f3e499ada1867a904e9327c</anchor>
      <arglist>(Store_ store)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scan_real</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a886a3f96eefbfd7c40ea884f0e4e63a6</anchor>
      <arglist>(Store_ &amp;&amp;store)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scan_double</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a820274a84266ec42a56946faa9a14997</anchor>
      <arglist>(Store_ store)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scan_complex</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a227f3f6dc4fa76da4a58c9cde71edd85</anchor>
      <arglist>(Store_ store)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scan_pattern</name>
      <anchorfile>classeminem_1_1Parser.html</anchorfile>
      <anchor>a87b392fda6246cbfc0cc1a8789bd6550</anchor>
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
      <name>block_size</name>
      <anchorfile>structeminem_1_1ParserOptions.html</anchorfile>
      <anchor>a0ee3f452160d3f1276ac2e66b7cc65f1</anchor>
      <arglist></arglist>
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
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structeminem_1_1ParseSomeBufferOptions.html</anchorfile>
      <anchor>a984ec0e4212279584de41cf7c6cfceed</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::size_t</type>
      <name>block_size</name>
      <anchorfile>structeminem_1_1ParseSomeBufferOptions.html</anchorfile>
      <anchor>a14c5a733ce2fe003714b15a2d1d49bb5</anchor>
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
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structeminem_1_1ParseSomeFileOptions.html</anchorfile>
      <anchor>a6d88951fbdd77f58d3b97ac84545ccfe</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::size_t</type>
      <name>block_size</name>
      <anchorfile>structeminem_1_1ParseSomeFileOptions.html</anchorfile>
      <anchor>a19c42cfcf728524d97471ee83f7bfa38</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>eminem::ParseTextBufferOptions</name>
    <filename>structeminem_1_1ParseTextBufferOptions.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structeminem_1_1ParseTextBufferOptions.html</anchorfile>
      <anchor>a9f8aabd2263c17a7386de4af1680068b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::size_t</type>
      <name>block_size</name>
      <anchorfile>structeminem_1_1ParseTextBufferOptions.html</anchorfile>
      <anchor>ac300ca7e16017eb13285b74e41194948</anchor>
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
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structeminem_1_1ParseTextFileOptions.html</anchorfile>
      <anchor>a70d6db2d9a0221b24dbe07824028f1ba</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::size_t</type>
      <name>block_size</name>
      <anchorfile>structeminem_1_1ParseTextFileOptions.html</anchorfile>
      <anchor>a6d36041585fd1a977bfd97d9aa53b2ed</anchor>
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
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structeminem_1_1ParseZlibBufferOptions.html</anchorfile>
      <anchor>a7edd35d6ce5666654c1a2784d823d8dd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::size_t</type>
      <name>block_size</name>
      <anchorfile>structeminem_1_1ParseZlibBufferOptions.html</anchorfile>
      <anchor>a91ae1f632d974dc6f373aa9939461987</anchor>
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
    <class kind="struct">eminem::ParserOptions</class>
    <class kind="struct">eminem::ParseSomeBufferOptions</class>
    <class kind="struct">eminem::ParseSomeFileOptions</class>
    <class kind="struct">eminem::ParseTextBufferOptions</class>
    <class kind="struct">eminem::ParseTextFileOptions</class>
    <class kind="struct">eminem::ParseZlibBufferOptions</class>
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
      <type>Parser&lt; byteme::PerByteSerial&lt; char &gt;, Index_ &gt;</type>
      <name>parse_gzip_file</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>a907387b09a970b7d27a2b9a60db846d2</anchor>
      <arglist>(const char *path, const ParseGzipFileOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Parser&lt; byteme::PerByteSerial&lt; char &gt;, Index_ &gt;</type>
      <name>parse_zlib_buffer</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>a295c0726c5c90fb6d3b64332221ece36</anchor>
      <arglist>(const unsigned char *buffer, std::size_t len, const ParseZlibBufferOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Parser&lt; byteme::PerByteSerial&lt; char &gt;, Index_ &gt;</type>
      <name>parse_some_file</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>aa96bca5a6f809e792ddc1e96e0fe0cfc</anchor>
      <arglist>(const char *path, const ParseSomeFileOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Parser&lt; byteme::PerByteSerial&lt; char &gt;, Index_ &gt;</type>
      <name>parse_some_buffer</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>a061b7a113d8f224fcebb6e8fe5882342</anchor>
      <arglist>(const unsigned char *buffer, std::size_t len, const ParseSomeBufferOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Parser&lt; byteme::PerByteSerial&lt; char &gt;, Index_ &gt;</type>
      <name>parse_text_file</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>a896794260d133ec3a4116348a205fd97</anchor>
      <arglist>(const char *path, const ParseTextFileOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Parser&lt; byteme::PerByteSerial&lt; char &gt;, Index_ &gt;</type>
      <name>parse_text_buffer</name>
      <anchorfile>namespaceeminem.html</anchorfile>
      <anchor>ad6da882a39fcb6e3bc36638b28910680</anchor>
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
