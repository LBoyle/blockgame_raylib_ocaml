open Constants;

// Because blocks are a 1d array this will select
// blocks that are wrapped, on the other side of the chunk
let get_block_neighbor_ids = b => {
  (
    b - 1,
    b + 1,
    b - chunkSize,
    b + chunkSize,
    b - chunkSize * chunkSize,
    b + chunkSize * chunkSize,
  );
};

let gen_chunk_mesh = (_ci: int, _chunk: array(Block.t)) => {
  ();
  ();
};
