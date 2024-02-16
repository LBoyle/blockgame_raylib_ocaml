open Raylib;
open Constants;

let index_to_3d_coord = b =>
  Vector3.create(
    float_of_int(b mod chunkSize),
    float_of_int(b / (chunkSize * chunkSize)),
    float_of_int(b / chunkSize mod chunkSize),
  );

let index_to_2d_coord = ci =>
  Vector3.create(
    float_of_int(ci mod worldSizeInChunks * chunkSize),
    0.,
    float_of_int(ci / worldSizeInChunks * chunkSize),
  );

let clamp_index_opt = i =>
  switch (i) {
  | n when n < 0 => None
  | n when n >= worldSizeInChunks * worldSizeInChunks => None
  | n => Some(n)
  };

let get_active_chunks_ids = (pos: Vector3.t) => {
  let (x, z) = (
    (Vector3.x(pos) |> floor |> int_of_float) / chunkSize,
    (Vector3.z(pos) |> floor |> int_of_float) / chunkSize,
  );
  // Player chunk index
  let pidx = x + z * worldSizeInChunks;
  // FIX ME
  [
    pidx - worldSizeInChunks - worldSizeInChunks,
    pidx - worldSizeInChunks - 1,
    pidx - worldSizeInChunks,
    pidx - worldSizeInChunks + 1,
    pidx - 2,
    pidx - 1,
    pidx,
    pidx + 1,
    pidx + 2,
    pidx + worldSizeInChunks - 1,
    pidx + worldSizeInChunks,
    pidx + worldSizeInChunks + 1,
    pidx + worldSizeInChunks + worldSizeInChunks,
  ]
  |> List.filter_map(clamp_index_opt);
};
