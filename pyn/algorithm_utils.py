#!/usr/bin/env python3
# coding=utf-8

# =====================================================复习大纲======================================================
# 排序种类：
#       冒泡排序，选择排序，插入排序，快速排序，堆排序，归并排序，希尔排序，桶排序，基数排序
# ===================================================================================================================
# 图：
#       有向图，无向图，有环图，无环图，完全图，稠密图，稀疏图，拓扑图
# ===================================================================================================================
# 路径：
#       最长路径，最短路径，单源路径，所有节点对路经
#       二叉树，红黑树，AVL树，van Emde Boas树，最小生成树
# ===================================================================================================================

import random


test_value = 0


def generate_random_array(number):
    global test_value
    array = []
    for i in range(number):
        array.append((random.randint( 0, 32767), i))
    test_value = array[number - 1]
    return array


# 二路归并排序
def merge_sort(array):
    pass


# 堆排序
def heap_sort(array):
    # 筛运算（在heap_sort函数内定义）
    def shift(data, size, idx):
        temp = data[idx]
        # idx的左孩子
        child = 2 * idx + 1
        while child < size:
            # 建立大根堆（如果右孩子大于左孩子，则取右孩子）
            if child < size - 1 and data[child] < data[child + 1]:
                child += 1
            if temp >= data[child]:
                break
            data[idx] = data[child]
            idx = child
            child = 2 * idx + 1
        data[idx] = temp

    length = len(array)
    # 建立堆
    i = int(length / 2)
    while i >= 0:
        shift(array, length, i)
        i -= 1
    # 堆排序
    for j in range(1, length):
        array[0], array[length - j] = array[length - j], array[0]
        shift(array, length - j, 0)
    return array


# 希尔排序（对每个间隔为一定的数组元素进行直接插入排序）
def shell_sort(array):
    step = len(array) / 2
    while step >= 1:
        step = int(step)
        for i in range(step, len(array)):
            temp = array[i]
            j = i - step
            while j >= 0 and temp < array[j]:
                array[j + step] = array[j]
                j -= step
            if i != j + step:
                array[j + step] = temp
        step /= 2
    return array


# 冒泡排序(交换排序的一种)
def bubble_sort(array):
    for i in range(1, len(array)):
        flag = False
        j = len(array) - 1
        while j >= i:
            if array[j - 1] > array[j]:
                array[j - 1], array[j] = array[j], array[j - 1]
                flag = True
            j -= 1
        # 如果本趟没发生过交换，则序列全部有序，不需要继续冒泡
        if not flag:
            return array
    return array


# 选择排序 low 写法（from c++）
def select_sort(array):
    for i in range(len(array)):
        k = i
        # [i+1,len(array))
        for j in range(i + 1, len(array)):
            if array[k] > array[j]:
                k = j
        if i != k:
            # python 这个交换写法写起来挺爽的，省时省事
            array[i], array[k] = array[k], array[i]
    return array


# 插入排序
def insert_sort(array):
    if len(array) < 2:
        return array
    # i 指示后端未排序列的head位置
    #  0          <- i             len(array)
    # |=============|**************|
    for i in range(1, len(array)):
        temp = array[i]
        # 从已排好序的列表尾部开始进行比较插入，方便移动元素
        j = i - 1
        while j >= 0 and temp < array[j]:
            # 元素往后一个位置搬移
            array[j + 1] = array[j]
            j -= 1
        if i != j + 1:
            array[j + 1] = temp
    return array


def insert_sort_by_binary(array):
    for i in range(1, len(array)):
        temp = array[i]
        start, end = 0, i - 1
        while start <= end:
            mid = int((start + end) / 2)
            if array[mid] < temp:
                start = mid + 1
            else:
                end = mid - 1
        for j in range(i - 1, end, -1):
            array[j + 1] = array[j]
        if end + 1 < i:
            array[end + 1] = temp
    return array


# 二分查找（循环查找法）
def binary_search_by_circulation(array, value):
    start, end = 0, len(array) - 1
    while start <= end:
        index = int((start + end) / 2)
        if value == array[index]:
            return index
        elif value > array[index]:
            start = index + 1
        else:
            end = index - 1
    return -1


# 二分查找（递归查找法）
def binary_search_by_recursion(array, value):
    def recursion(start, end):
        if start > end:
            return -1
        index = int((start + end) / 2)
        if value == array[index]:
            return index
        # python 中的三目运算符
        start, end = (index + 1, end) if value > array[index] else (start, index - 1)
        return recursion(start, end)

    return recursion(0, len(array) - 1)


# 分别从两端找>和<进行交换，求出分割的中间位置
def part_split(array, beg, end):
    if beg > end:
        return
    idx, beg = beg, beg + 1
    while beg <= end:
        while beg <= end and array[beg] <= array[idx]:
            beg += 1
        while beg <= end and array[end] >= array[idx]:
            end -= 1
        if beg < end:
            array[beg], array[end] = array[end], array[beg]
            beg, end = beg + 1, end - 1
    if idx != beg - 1:
        array[idx], array[beg - 1] = array[beg - 1], array[idx]
    return beg - 1


def part_split_2(array, beg, end):
    temp = array[beg]
    while beg < end:
        while beg < end and temp <= array[end]:
            end -= 1
        array[beg] = array[end]
        while beg < end and temp >= array[beg]:
            beg += 1
        array[end] = array[beg]
    array[beg] = temp
    return beg


# 快速排序（C++版）
def quick_sort_by_cpp(array):
    def quick_sort_by_split(data, start, end):
        if end - start <= 0:
            return data
        mid = part_split(data, start, end)
        quick_sort_by_split(data, start, mid - 1)
        quick_sort_by_split(data, mid + 1, end)
        return data

    if len(array) <= 1:
        return array
    return quick_sort_by_split(array, 0, len(array) - 1)


# 快速排序（递归，python版）（D&C（分而治之）思想的运用）
def quick_sort(array):
    if len(array) < 2:
        return array
    else:
        pivot = array[0]
        # 下面的语法没见过，有点牛逼啊
        less = [i for i in array[1:] if i <= pivot]
        greater = [i for i in array[1:] if i > pivot]
        return quick_sort(less) + [pivot] + quick_sort(greater)


def rotate_left(array, offset):
    size = len(array)
    if size % offset == 0:
        i = 0
        while i < offset:
            temp = array[i]
            j = i + offset
            while j < size:
                array[j - offset] = array[j]
                j += offset
            array[j - offset] = temp
            i += 1
    else:
        temp = array[0]
        j = 0
        for i in range(size):
            array[j % size] = array[(j + offset) % size]
            j += offset
        array[(j - offset) % size] = temp


if __name__ == '__main__':
    arr = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20]
    print(arr)
    rotate_left(arr, 11)
    print(arr)

    # arr_arg = [40, 100, 3, 90, 76, 53, 100, 2, 46, 34, 29, 55, 108, 0, 9]
    # arr_arg = generate_random_array(50)
    # print("原来数组是：")
    # print(arr_arg)
    # print("排序之后数组是：")
    # arr = arr_arg.copy()
    # print(bubble_sort(arr))
    # arr = arr_arg.copy()
    # print(quick_sort(arr))
    # arr = arr_arg.copy()
    # print(select_sort(arr))
    # arr = arr_arg.copy()
    # print(insert_sort(arr))
    # arr = arr_arg.copy()
    # print(insert_sort_by_binary(arr))
    # random.shuffle(arr)
    # print("打乱之后数组是：")
    # print(arr)
    # print("排序之后数组是：")
    # print(shell_sort(arr))

    #test_array = [0, 2, 1]
    #arr = test_array[:]
    #print(part_split(arr, 0, 2))
    #print(arr)
    #arr = test_array[:]
    #print(part_split_2(arr, 0, 2))
    #print(arr)
    #test_array = [8, 6, 1, 10, 13, 5, 2, 19, 24, 0, 11, 8]
    #random.shuffle(test_array)
    #print(heap_sort(test_array))
#
    # a, b, c = (24504, 0), test_value, test_value
    # print(test_value)
    # print(binary_search_by_circulation(arr, a))
    # print(binary_search_by_circulation(arr, b))
    # print(binary_search_by_recursion(arr, a))
    # print(binary_search_by_recursion(arr, b))


