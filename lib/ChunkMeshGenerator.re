let get_block_neighbor_ids = b => {
  (
    b - 1,
    b + 1,
    b - Constants.chunkSize,
    b + Constants.chunkSize,
    b - Constants.chunkSize * Constants.chunkSize,
    b + Constants.chunkSize * Constants.chunkSize,
  );
};

let gen_chunk_mesh = (_ci, _chunk: array(Block.t)) => {
  ();
  ();
};
