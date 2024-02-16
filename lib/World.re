let cs = Constants.chunkSize;

let genGrassBlock = () => {
  switch (Random.int(10)) {
  | n when n > 5 => Block.Grass
  | _ => Air
  };
};

let generate_chunk = _ci => {
  Array.make(cs * cs, Block.Air)
  |> Array.append(
       Array.make(cs * cs, Block.Air) |> Array.map(_ => genGrassBlock()),
     )
  |> Array.append(Array.make(cs * cs, Block.Dirt));
};
