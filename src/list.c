#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ghh/list.h>

typedef struct list_node {
	void *item;
	struct list_node *next;
} listnode_t;

struct ghh_list {
	listnode_t *root, *tip;
	size_t size;
};

struct ghh_listiter {
	list_t *list;
	listnode_t *node;
};

list_t *list_create() {
	list_t *list = malloc(sizeof(*list));

	list->root = NULL;
	list->tip = NULL;
	list->size = 0;

	return list;
}

void list_destroy(list_t *list, bool destroy_values) {
	listnode_t *trav, *last;

	trav = list->root;

	while (trav != NULL) {
		last = trav;
		trav = trav->next;

		if (destroy_values)
			free(last->item);

		free(last);
	}

	free(list);
}

size_t list_size(list_t *list) {
	return list->size;
}

void *list_get(list_t *list, size_t index) {
	if (index >= list->size) {
		printf("tried to access node at index outside of linked list.\n");
		exit(1);
	}

	int i = 0;
	listnode_t *trav = list->root;

	while (++i < index)
		trav = trav->next;

	return trav;
}

void list_push(list_t *list, void *item) {
	listnode_t *node = malloc(sizeof(listnode_t));

	node->item = item;

	if (list->root == NULL) {
		node->next = NULL;
		list->root = node;
		list->tip = node;
	} else {
		node->next = list->root;
		list->root = node;
	}

	++list->size;
}

void list_append(list_t *list, void *item) {
	listnode_t *node = malloc(sizeof(listnode_t));

	node->item = item;
	node->next = NULL;

	if (list->size) {
		list->tip->next = node;
		list->tip = node;
	} else {
		list->root = node;
		list->tip = node;
	}

	++list->size;
}

void *list_pop(list_t *list) {
	if (list->root == NULL) {
		printf("attempted to pop from empty list.\n");
		exit(1);
	}

	listnode_t *old_root;
	void *item;

	old_root = list->root;
	item = old_root->item;
	list->root = old_root->next;

	free(old_root);

	if (--list->size == 0)
		list->tip = NULL;

	return item;
}

void *list_peek(list_t *list) {
	return list->root->item;
}

void *list_remove(list_t *list, void *item) {
	listnode_t *trav, *last;

	last = NULL;
	trav = list->root;

	while (trav != NULL && trav->item != item) {
		last = trav;
		trav = trav->next;
	}

	if (trav != NULL) {
		void *item;

		if (trav == list->root) {
			list->root = trav->next;
		} else if (trav == list->tip) {
			list->tip = last;
			last->next = NULL;
		} else {
			last->next = trav->next;
		}

		item = trav->item;
		free(trav);
		--list->size;

		return item;
	}

	return NULL;
}

void list_merge(list_t *list, list_t *other) {
	if (list->size)
		list->tip->next = other->root;
	else
		list->root = other->root;

	list->tip = other->tip;
	list->size += other->size;

	other->root = NULL;
	other->tip = NULL;
	other->size = 0;
}

listiter_t *list_iter_create(list_t *list) {
	listiter_t *iter = malloc(sizeof(*iter));

	iter->list = list;
	iter->node = NULL;

	return iter;
}

void list_iter_reset(listiter_t *iter) {
	iter->node = NULL;
}

void *list_iter_next(listiter_t *iter) {
	if (iter->node == NULL)
		iter->node = iter->list->root;
	else
		iter->node = iter->node->next;

	return iter->node == NULL ? NULL : iter->node->item;
}
