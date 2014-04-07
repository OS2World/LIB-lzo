
Note:
=====

  For reasons of speed all assembler decompressors can access (write to)
  up to 3 bytes past the end of the decompressed (output) block.
  Data past the end of the compressed (input) block is never
  accessed (read from).

  If this is not acceptable in your situation (say you're directly
  uncompressing to video memory), you can shorten the block by
  3 bytes when compressing and manually copy those last 3 bytes
  after uncompressing.

