package main

import (
	"fmt"
	"os"
)

type LRUNode struct {
	pre  *LRUNode
	next *LRUNode
	key  interface{}
	val  interface{}
}

func NewLRUNode(key, val interface{}) *LRUNode {
	n := &LRUNode{nil, nil, key, val}
	n.pre = n
	n.next = n
	return n
}

func (n *LRUNode) del() {
	n.pre.next = n.next
	n.next.pre = n.pre
}

func (n *LRUNode) insert(node *LRUNode) {
	node.pre = n
	node.next = n.next
	n.next.pre = node
	n.next = node
}

/**********************************************/

type LRU struct {
	size     int
	nodes    map[interface{}]*LRUNode
	*LRUNode // 带头结点循环链表
}

func NewLRU(size int) *LRU {
	if size <= 0 {
		return nil
	}

	l := &LRU{size: size,
		nodes:   make(map[interface{}]*LRUNode),
		LRUNode: NewLRUNode(struct{}{}, struct{}{}),
	}
	return l
}

func (l *LRU) Add(key, val interface{}) {
	if node, ok := l.nodes[key]; ok {
		node.val = val
		node.del()
		l.insert(node)
		return
	}

	if len(l.nodes) >= l.size {
		delete(l.nodes, l.pre.key)
		l.pre.del()
	}

	node := NewLRUNode(key, val)
	l.nodes[key] = node
	l.insert(node)
}

func (l *LRU) Get(key interface{}) (interface{}, bool) {
	node, ok := l.nodes[key]
	if !ok {
		return nil, false
	}
	node.del()
	l.insert(node)
	return node.val, true
}

/**********************************************/
func assert(exp, val interface{}) {
	if exp != val {
		fmt.Printf("exp:%+v, get:%+v\n", exp, val)
		os.Exit(-1)
	}
}

func main() {
	l := NewLRU(2)
	l.Add(1, 1)
	l.Add(2, 2)

	v, ok := l.Get(1)
	assert(true, ok)
	assert(1, v)
	v, ok = l.Get(2)
	assert(true, ok)
	assert(2, v)

	_, ok = l.Get(3)
	assert(false, ok)

	l.Add(3, "hello")
	_, ok = l.Get(1)
	assert(false, ok)

	v, ok = l.Get(3)
	assert(true, ok)
	assert("hello", v)
}
