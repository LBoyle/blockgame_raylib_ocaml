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
  let playerChunk = x + z * worldSizeInChunks;
  // FIX ME
  let res =
    [
      playerChunk - worldSizeInChunks - worldSizeInChunks,
      playerChunk - worldSizeInChunks - 1,
      playerChunk - worldSizeInChunks,
      playerChunk - worldSizeInChunks + 1,
      playerChunk - 2,
      playerChunk - 1,
      playerChunk,
      playerChunk + 1,
      playerChunk + 2,
      playerChunk + worldSizeInChunks - 1,
      playerChunk + worldSizeInChunks,
      playerChunk + worldSizeInChunks + 1,
      playerChunk + worldSizeInChunks + worldSizeInChunks,
    ]
    |> List.filter_map(x => clamp_index_opt(x));
  res;
};
