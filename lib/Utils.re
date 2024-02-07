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

let clamp_index = i =>
  if (i < 0) {
    0;
  } else if (i >= worldSizeInChunks * worldSizeInChunks) {
    worldSizeInChunks * worldSizeInChunks;
  } else {
    i;
  };

let clamp_index_opt = i =>
  if (i < 0) {
    None;
  } else if (i >= worldSizeInChunks * worldSizeInChunks) {
    None;
  } else {
    Some(i);
  };
