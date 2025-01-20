# TODO:

- Move getadjacentchunk logic to refreshchunkscenteredat since it is not thread safe
	- This causes stutter now - perf improvements?
	- code complexity is up - would going back to prior code and just making chunks thread safe be better?
	- Getting rid of relative coords might be a good approach - instead each chunk can compute its own offset from the world coor?
- Why does moving fast get ahead of chunk queue eventually?
- View frustrum culling for performance
- Save chunks to disk once they've been loaded
