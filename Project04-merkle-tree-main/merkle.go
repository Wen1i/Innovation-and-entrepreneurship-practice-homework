//节点结构体定义
type MerkleNode struct {
   Left  *MerkleNode
   Right *MerkleNode
   Data  []byte
}

//树的结构体定义
type MerkleTree struct {
   RootNode *MerkleNode
}

//创建节点
func NewMerkleNode(left, right *MerkleNode, data []byte) *MerkleNode {
   mNode := MerkleNode{}
   //如果左右子树为空，那么data就是原始数据节点
   if left == nil && right == nil {
      //计算哈希
      hash := sha256.Sum256(data)
      mNode.Data = hash[:]
   } else {
      //将左右子树的数据集合在一起
      preHashes := append(left.Data, right.Data...)
      //计算哈希
      hash := sha256.Sum256(preHashes)
      mNode.Data = hash[:]
   }
   //左右子树赋值
   mNode.Left = left
   mNode.Right = right

   return &mNode
}

func NewMerkleTree(data [][]byte) *MerkleTree {
   var nodes []MerkleNode
   //如果是单数个节点，复制最后一个节点
   if len(data)%2 != 0 {
      data = append(data, data[len(data)-1])
   }
   //构造节点
   for _, datum := range data {
      node := NewMerkleNode(nil, nil, datum)
      nodes = append(nodes, *node)
   }
   //层层计算
   for i := 0; i < len(data)/2; i++ {
      var newLevel []MerkleNode
      //相邻两个节点合并
      for j := 0; j < len(nodes); j += 2 {
         node := NewMerkleNode(&nodes[j], &nodes[j+1], nil)
         newLevel = append(newLevel, *node)
      }
      nodes = newLevel
   }
   mTree := MerkleTree{&nodes[0]}
   return &mTree
}

//节点信息打印函数
func PrintNode(node *MerkleNode) {
   fmt.Printf("%p\n", node)
   if node != nil {
      fmt.Printf("left[%p], right[%p], data(%x)\n", node.Left, node.Right, node.Data)
      fmt.Printf("check:%t\n", check(node))
   }
}

//先序遍历
func showMerkleTree(root *MerkleNode) {
   if root == nil {
      return
   } else {
      //打印节点信息
      PrintNode(root)
   }
   showMerkleTree(root.Left)
   showMerkleTree(root.Right)
}

//检查结果是否正确
func check(node *MerkleNode) bool {
   if node.Left == nil {
      return true
   }
   //左右两个子树的data数值联合起来计算hash值并与父节点的data进行比较
   prevHashes := append(node.Left.Data, node.Right.Data...)
   hash32 := sha256.Sum256(prevHashes)
   hash := hash32[:]
   return bytes.Compare(hash, node.Data) == 0
}
