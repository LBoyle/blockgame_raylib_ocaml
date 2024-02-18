type t =
  | Air
  | Dirt
  | Stone
  | Grass
  | Water;
let get_block_colour = block =>
  switch (block) {
  | Air => None
  | Dirt => Some(Raylib.Color.brown)
  | Stone => Some(Raylib.Color.gray)
  | Grass => Some(Raylib.Color.lime)
  | Water => Some(Raylib.Color.blue)
  };
