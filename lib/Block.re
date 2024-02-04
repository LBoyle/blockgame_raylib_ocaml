type t =
  | Air
  | Dirt
  | Grass
  | Water;
let get_block_colour = block =>
  switch (block) {
  | Air => None
  | Dirt => Some(Raylib.Color.brown)
  | Grass => Some(Raylib.Color.lime)
  | Water => Some(Raylib.Color.blue)
  };
