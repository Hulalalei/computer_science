public class TreeNode {
    private int data;
    private TreeNode pre;
    private TreeNode next;


    public TreeNode() {
    }

    public TreeNode(int data, TreeNode pre, TreeNode next) {
        this.data = data;
        this.pre = pre;
        this.next = next;
    }

    /**
     * 获取
     * @return data
     */
    public int getData() {
        return data;
    }

    /**
     * 设置
     * @param data
     */
    public void setData(int data) {
        this.data = data;
    }

    /**
     * 获取
     * @return pre
     */
    public TreeNode getPre() {
        return pre;
    }

    /**
     * 设置
     * @param pre
     */
    public void setPre(TreeNode pre) {
        this.pre = pre;
    }

    /**
     * 获取
     * @return next
     */
    public TreeNode getNext() {
        return next;
    }

    /**
     * 设置
     * @param next
     */
    public void setNext(TreeNode next) {
        this.next = next;
    }

    public String toString() {
        return "TreeNode{data = " + data + ", pre = " + pre + ", next = " + next + "}";
    }
}