let cs = Constants.chunkSize;

let generate_chunk = () => {
  Array.make(cs * cs, Block.Air)
  |> Array.append(Array.make(cs * cs, Block.Grass))
  |> Array.append(Array.make(cs * cs, Block.Dirt));
};

let initial_blocks = () => {
  Array.make(cs * cs, Block.Air)
  |> Array.append(Array.make(cs * cs, Block.Grass))
  |> Array.append(Array.make(cs * cs, Block.Dirt));
};
