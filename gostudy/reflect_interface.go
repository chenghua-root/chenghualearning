package main

import (
	"errors"
	"fmt"
	"reflect"
)

/*
 1. interface与Value和Type之间可以通过函数来打通

   Interface-------------reflect.TypeOf()--------------->Type
     ^   |                                                ^
     |	 |                                                |
     |	 |                                                |Value.Type()
	 |   |                                                |
	 |   |                                                |
	 |	 |                                                |
	 |	 --------------refelect.ValueOf()--------->Value---
	 |                                                |
	 |                                                |
	 |                                                |
	 ------------------Value.Interface()---------------

*/

/**********************************例子1***************************/
/*
 * 来源https://mp.weixin.qq.com/s/Hke0mSCEa4ga_GS_LUp78A
 */
type Child struct {
	Name  string
	Grade int

	Handsome bool
}

type Adult struct {
	ID         string `qson:"Name"`
	Occupation string
	Handsome   bool
}

// 如果输入参数 i 是 Slice，元素是结构体，有一个字段名为 `Handsome`，
// 并且有一个字段的 tag 或者字段名是 `Name` ，
// 如果该 `Name` 字段的值是 `qcrao`，
// 就把结构体中名为 `Handsome` 的字段值设置为 true。
func handsome(i interface{}) {
	// 获取 i 的反射变量 Value
	v := reflect.ValueOf(i)

	// 确定 v 是一个 Slice
	if v.Kind() != reflect.Slice {
		return
	}

	/* 获取元素的类型
	 * 为了获取元素的类型，需要通过Type()函数获取v的type信息，再通过Elem() Type获取元素的类型 \
	 * It panics if the type's Kind is not Array, Chan, Map, Ptr, or Slice.
	 */
	st := v.Type().Elem()

	// 确定元素类型为结构体
	if st.Kind() != reflect.Struct {
		return
	}

	/*
	 * 结构体类型对象实现了NumField()
	 */
	foundName := false
	for i := 0; i < st.NumField(); i++ {
		f := st.Field(i)
		tag := f.Tag.Get("qson")
		// 确认有成员名字为Name或其qson标签值为Name
		if (f.Name == "Name" || tag == "Name") && f.Type.Kind() == reflect.String {
			foundName = true
			break
		}
	}

	if !foundName {
		return
	}

	// 确认包含Handsome成员并且为bool类型
	if niceField, foundHandsome := st.FieldByName("Handsome"); foundHandsome == false || niceField.Type.Kind() != reflect.Bool {
		return
	}

	for i := 0; i < v.Len(); i++ {
		e := v.Index(i)
		handsome := e.FieldByName("Handsome")

		// 寻找字段名为 Name 或者 tag 的值为 Name 的字段
		var name reflect.Value
		for j := 0; j < st.NumField(); j++ {
			f := st.Field(j)
			tag := f.Tag.Get("qson")
			if tag == "Name" || f.Name == "Name" {
				name = v.Index(i).Field(j)
			}
		}

		// 设置名字为 "qcrao" 的对象的 "Handsome" 字段为 true
		if name.String() == "qcrao" {
			handsome.SetBool(true)
		}
	}
}

func handsomeTest() {
	children := []Child{
		{Name: "Ava", Grade: 3, Handsome: false},
		{Name: "qcrao", Grade: 6, Handsome: false},
	}

	adults := []Adult{
		{ID: "Steve", Occupation: "Clerk", Handsome: false},
		{ID: "qcrao", Occupation: "Go Programmer", Handsome: false},
	}

	fmt.Printf("children before handsome: %v\n", children)
	handsome(children)
	fmt.Printf("children after handsome: %v\n", children)

	fmt.Println()

	fmt.Printf("adults before handsome: %v\n", adults)
	handsome(adults)
	fmt.Printf("adults after handsome: %v\n", adults)

	fmt.Println("-------------")
}

/**********************************例子2***************************/

type CStudent struct {
	Name   string
	Age    int
	Addr   string
	Scores []int
}

type SStudent struct {
	Name   string
	Age    int
	Hobby  string
	Scores []int
}

/*
 * 拷贝结构体来实现赋值，当成员一致时(成员名&类型)进行拷贝
 * 当类型为slice时，需要深度拷贝，即目的slice需要分配空间
 */
func copyStruct(src interface{}, dst interface{}) error {
	if src == nil || dst == nil {
		return errors.New("nil")
	}

	vsrc := reflect.ValueOf(src)
	vsrc = reflect.Indirect(vsrc)

	if vsrc.Kind() != reflect.Struct {
		return errors.New("src not struct")
	}

	vdst := reflect.ValueOf(dst)
	vdst = reflect.Indirect(vdst)

	if vdst.Kind() != reflect.Struct {
		return errors.New("dst not struct")
	}
	if !vdst.CanSet() {
		return errors.New("dst not can set")
	}

	for i := 0; i < vdst.NumField(); i++ {
		tdstField := vdst.Type().Field(i).Type

		fieldName := vdst.Type().Field(i).Name

		vsrcField := vsrc.FieldByName(fieldName)
		if (vsrcField == reflect.Value{}) { // 不存在
			continue
		}

		if tdstField == vsrcField.Type() {
			if tdstField.Kind() == reflect.Slice {
				slice := reflect.MakeSlice(tdstField, vsrcField.Len(), vsrcField.Cap())
				reflect.Copy(slice, vsrcField)
				vdst.Field(i).Set(slice)
			} else {
				vdst.Field(i).Set(vsrcField)
			}
		}
	}

	return nil
}

func copyStructTest() {
	cs := CStudent{
		Name:   "Li",
		Age:    17,
		Addr:   "Hengdian",
		Scores: make([]int, 0, 0),
	}
	for i := 0; i < 3; i++ {
		cs.Scores = append(cs.Scores, i)
	}
	fmt.Printf("CStudent=%+v\n", cs)

	ss := SStudent{}
	err := copyStruct(&cs, &ss)
	if err != nil {
		fmt.Println(err)
	}
	fmt.Printf("SStudent=%+v\n", ss)
	fmt.Println()

	ss.Scores = append(ss.Scores, 10)
	fmt.Printf("CStudent=%+v\n", cs)
	fmt.Printf("SStudent=%+v\n", ss)
	fmt.Println("-------------")
}

func main() {
	handsomeTest()
	copyStructTest()
}
