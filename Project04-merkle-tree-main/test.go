func main() {
   data := [][]byte{
      []byte("node1"),
      []byte("node2"),
      []byte("node3"),
      []byte("node4"),
   }

   tree := NewMerkleTree(data)
   showMerkleTree(tree.RootNode)
}
