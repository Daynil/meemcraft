# TODO:

- Figure out issues with GetAdjacentChunks logic now that we're deleting chunks
	- How do we avoid deleting chunks with adjacency references?
	- Do we need to make chunks thread safe, or is it OK since read only in thread?
	- Possible solution just to make sure to always not delete chunks that are being referenced adjacent to another chunk in view?
- View frustrum culling for performance
- Save chunks to disk once they've been loaded
