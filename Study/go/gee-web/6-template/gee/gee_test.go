package gee

import "testing"

func TestNewGroup(t *testing.T) {
	engine := New()
	group1 := engine.Group("/v1")
	group2 := engine.Group("/v2")

	if group1.prefix != "/v1" {
		t.Errorf("Expected group1 prefix to be /v1, but got %s", group1.prefix)
	}

	if group2.prefix != "/v2" {
		t.Errorf("Expected group2 prefix to be /v2, but got %s", group2.prefix)
	}

	if len(engine.groups) != 3 { // including the default group
		t.Errorf("Expected engine to have 3 groups, but got %d", len(engine.groups))
	}
}