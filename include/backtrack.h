/**
 * @file backtrack.h
 *
 */

#ifndef BACKTRACK_H_
#define BACKTRACK_H_

#include "candidate_set.h"
#include "common.h"
#include "graph.h"

class Backtrack {
 public:
  Backtrack();
  ~Backtrack();

  void PrintAllMatches(const Graph &data, const Graph &query,
                       const CandidateSet &cs);
 private:
  std::vector<std::vector<Vertex> > dag_query_edge;
  std::vector<std::vector<Vertex> > dag_query_edge_inv;
  std::vector<size_t> dag_query_indegree;
  std::vector<std::vector<std::vector<std::pair<Vertex, Vertex> > > > dag_candidate_edge;
  std::vector<size_t> dag_query_getdegree;
  std::vector<std::vector<size_t> > dag_candidate_getdegree;
  std::vector<Vertex> selected_vertex;
  void DoSelect(const Graph &data, const Graph &query, const CandidateSet &cs);
  std::vector<size_t> full_indegree;
  size_t ans_num;
  std::vector<Vertex> ans;
};

#endif  // BACKTRACK_H_
