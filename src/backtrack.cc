/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"

Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}

void Backtrack::DoSelect(const Graph &data, const Graph &query,
                                const CandidateSet &cs) {
  // find 100000 answer
  if(ans_num >= 100000) return;

  size_t qs = query.GetNumVertices();

  Vertex sv = -1;

  // choose selected vertex that minimum valid candidate size and prefer have at least 1 out degree
  size_t mcs = data.GetNumVertices() * 2 + 1;
  size_t mv;
  for(size_t i = 0; i < qs; i++) if((selected_vertex[i]) == -1 && (dag_query_getdegree[i] == dag_query_indegree[i])) {
    mv = full_indegree[i];
    if((mv != 0) && (dag_query_edge[i].size() == 0)) mv += data.GetNumVertices();
    if(mv < mcs) {
      sv = i;
      mcs = mv;
    }
  }

  // there is no valid vertex -> traverse end
  if(sv == -1) {
    for(size_t i = 0; i < qs; i++) if(selected_vertex[i] == -1) {
      return;
    }

    // test print
    /*if(ans_num < 5) {
      std::cout << "a";
      for(size_t i = 0; i < qs; i++) std::cout << " " << selected_vertex[i];
      std::cout << "\n";
    }*/

    // print answer
    std::cout << "a";
    for(size_t i = 0; i < qs; i++) std::cout << " " << selected_vertex[i];
    std::cout << "\n";

    // check valid
    /*bool valid = true;
    for(size_t i = 0; (i < qs) && valid; i++) {
      if(query.GetLabel(i) != data.GetLabel(selected_vertex[i])) valid = false;
      for(size_t j = i + 1; (j < qs) && valid; j++) {
        if(selected_vertex[i] == selected_vertex[j]) valid = false;
        if(query.IsNeighbor(i, j)) {
          if(!data.IsNeighbor(selected_vertex[i], selected_vertex[j])) valid = false;
        }
      }
    }
    if(!valid) std::cout << "Invalid answer!" << "\n";*/

    ans_num++;

    return;
  }

  
  // loop vertex in candidate set of sv
  for(size_t i = 0; i < cs.GetCandidateSize(sv); i++) {
    if(dag_candidate_getdegree[sv][i] == dag_query_indegree[sv] && (!is_selected[cs.GetCandidate(sv, i)])) {

      selected_vertex[sv] = cs.GetCandidate(sv, i);
      is_selected[selected_vertex[sv]] = true;


      // apply candidate edge
      for(size_t j = 0; j < dag_candidate_edge[sv][i].size(); j++) {
        Vertex qv = dag_candidate_edge[sv][i][j].first;
        Vertex qvd = dag_candidate_edge[sv][i][j].second;
        if(!is_selected[cs.GetCandidate(qv, qvd)]) {
          dag_candidate_getdegree[qv][qvd]++;
          if(dag_candidate_getdegree[qv][qvd] == dag_query_indegree[qv]) {
            full_indegree[qv]++;
          }
        }
      }

      // apply query edge
      for(size_t j = 0; j < dag_query_edge[sv].size(); j++) {
        Vertex qv = dag_query_edge[sv][j];
        dag_query_getdegree[qv]++;
      }

      ans.push_back(sv);

      // recursive
      DoSelect(data, query, cs);


      // restore for backtrack
      ans.pop_back();

      for(size_t j = 0; j < dag_query_edge[sv].size(); j++) {
        Vertex qv = dag_query_edge[sv][j];
        dag_query_getdegree[qv]--;
      }

      for(size_t j = 0; j < dag_candidate_edge[sv][i].size(); j++) {
        Vertex qv = dag_candidate_edge[sv][i][j].first;
        Vertex qvd = dag_candidate_edge[sv][i][j].second;
        if(!is_selected[cs.GetCandidate(qv, qvd)]) {
          if(dag_candidate_getdegree[qv][qvd] == dag_query_indegree[qv]) full_indegree[qv]--;
          dag_candidate_getdegree[qv][qvd]--;
        }
      }

      is_selected[selected_vertex[sv]] = false;

      selected_vertex[sv] = -1;
    }
  }
  return;
}

void Backtrack::PrintAllMatches(const Graph &data, const Graph &query,
                                const CandidateSet &cs) {
  std::cout << "t " << query.GetNumVertices() << "\n";

  // implement your code here.

  size_t qs = query.GetNumVertices();


  // init
  std::vector<bool> visited;
  visited.resize(qs, false);

  dag_query_edge.resize(qs);
  for(size_t i = 0; i < qs; i++) dag_query_edge[i].clear();

  dag_query_edge_inv.resize(qs);
  for(size_t i = 0; i < qs; i++) dag_query_edge_inv[i].clear();

  dag_query_indegree.resize(qs, 0);

  dag_candidate_edge.resize(qs);
  for(size_t i = 0; i < qs; i++) {
    dag_candidate_edge[i].resize(cs.GetCandidateSize(i));
    for(size_t j = 0; j < cs.GetCandidateSize(i); j++) dag_candidate_edge[i][j].clear();
  }

  dag_candidate_getdegree.resize(qs);
  for(size_t i = 0; i < qs; i++) dag_candidate_getdegree[i].resize(cs.GetCandidateSize(i), 0);

  selected_vertex.resize(qs, -1);
  
  full_indegree.resize(qs, 0);

  dag_query_getdegree.resize(qs, 0);

  is_selected.resize(data.GetNumVertices(), false);

  ans_num = 0;



  // make dag query and candidate
  std::vector<std::pair<size_t, Vertex> > ord;
  ord.clear();
  for(size_t i = 0; i < qs; i++) ord.push_back(std::make_pair(cs.GetCandidateSize(i), i));
  std::sort(ord.begin(), ord.end());

  for(size_t oi = 0; oi < qs; oi++) {
    Vertex i = ord[oi].second;
    visited[i] = true;
    for(size_t t = query.GetNeighborStartOffset(i); t < query.GetNeighborEndOffset(i); t++) {
      Vertex j = query.GetNeighbor(t);
      if(!visited[j]) {
        dag_query_edge[i].push_back(j);
        dag_query_edge_inv[j].push_back(i);
        dag_query_indegree[j]++;
        for(size_t ci = 0; ci < cs.GetCandidateSize(i); ci++) {
          for(size_t cj = 0; cj < cs.GetCandidateSize(j); cj++) {
            if(data.IsNeighbor(cs.GetCandidate(i, ci), cs.GetCandidate(j, cj))) {
              dag_candidate_edge[i][ci].push_back(std::make_pair(j, cj));
            }
          }
        }
      }
    }
  }

  // start vertex
  for(size_t i = 0; i < qs; i++) if(dag_query_indegree[i] == 0) {
    full_indegree[i] = cs.GetCandidateSize(i);
  }

  DoSelect(data, query, cs);

  /*std::cout << ans_num << "\n";
  if(ans_num == 100000) std::cout << "find 100000 matching!" << "\n";*/

  return;
}
