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
  size_t ds = data.GetNumVertices();

  Vertex sv = -1;

  // choose vertex in extendable vertex that has minimum extendable candidate size
  // prefer have at least 1 out-degree
  // prefer there is no in-degree
  // prefer many backtrack panelty count
  int32_t mcs = ds * 5 + qs + 1;
  int32_t mv;
  for(size_t i = 0; i < qs; i++) {
    // check extendable
    if((selected_vertex[i]) == -1 && (dag_query_getdegree[i] == dag_query_indegree[i])) {
      // score matching order priority
      mv = full_indegree[i];
      if(mv > 1) {
        if(dag_query_edge[i].size() == 0) mv += 2 *ds;
        if(dag_query_indegree[i] != 0) mv += 2 * ds;
      }
      mv -= back_cnt[i];

      if(mv < mcs) {
        sv = i;
        mcs = mv;
      }
    }
  }


  // there is no extendable vertex
  if(sv == -1) {
    for(size_t i = 0; i < qs; i++) if(selected_vertex[i] == -1) {
      return;
    }

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

    ans_num++;

    return;
  }

  // find extendable cs of sv
  std::vector<Vertex> extendable_cs;
  extendable_cs.clear();
  for(size_t i = 0; i < cs.GetCandidateSize(sv); i++) {
    if(dag_candidate_getdegree[sv][i] == dag_query_indegree[sv] && (!is_selected[cs.GetCandidate(sv, i)])) {
      if(only_v[cs.GetCandidate(sv, i)] == -1 || only_v[cs.GetCandidate(sv, i)] == sv) {
        extendable_cs.push_back(i);
      }
    }
  }

  // there is no extendable cs of sv
  if(extendable_cs.size() == 0) {
    // backtrack panelty count for matching order
    back_cnt[sv]++;
  }

  // loop extendable cs of sv
  for(size_t ei = 0; ei < extendable_cs.size(); ei++) {
    bool valid = true;

    Vertex i = extendable_cs[ei];
    selected_vertex[sv] = cs.GetCandidate(sv, i);
    is_selected[selected_vertex[sv]] = true;

    // init extendable cs size of adjecent vertex
    for(size_t j = 0; j < dag_query_edge[sv].size(); j++) {
      Vertex qv = dag_query_edge[sv][j];
      full_indegree[qv] = 0;
    }

    // apply candidate set edge
    for(size_t j = 0; j < dag_candidate_edge[sv][i].size(); j++) {
      Vertex qv = dag_candidate_edge[sv][i][j].first;
      Vertex qvd = dag_candidate_edge[sv][i][j].second;
      if(!is_selected[cs.GetCandidate(qv, qvd)]) {
        if(only_v[cs.GetCandidate(qv, qvd)] == -1 || only_v[cs.GetCandidate(qv, qvd)] == qv) {
          if(dag_candidate_getdegree[qv][qvd] == dag_query_getdegree[qv]) {
            full_indegree[qv]++;
          }
          dag_candidate_getdegree[qv][qvd]++;
        }
      }
    }

    // apply query edge
    for(size_t j = 0; j < dag_query_edge[sv].size(); j++) {
      Vertex qv = dag_query_edge[sv][j];
      dag_query_getdegree[qv]++;
      // check empty extendable cs
      if(full_indegree[qv] == 0) {
        valid = false;
      }
    }


    // delete selected vertex from other's extendable cs
    for(size_t j = 0; j < dq[selected_vertex[sv]].size(); j++) {
      if(dq[selected_vertex[sv]][j].first != sv) {
        Vertex dqv = dq[selected_vertex[sv]][j].first;
        size_t dqi = dq[selected_vertex[sv]][j].second;
        if(dag_candidate_getdegree[dqv][dqi] == dag_query_getdegree[dqv]) {
          full_indegree[dqv]--;
        }
      }
    }

    ans.push_back(sv);

    // recursive call
    if(valid) DoSelect(data, query, cs);
    else {
      // backtrack panelty count for matching order
      back_cnt[sv]++;
    }


    // restore for backtrack
    ans.pop_back();

    for(size_t j = 0; j < dq[selected_vertex[sv]].size(); j++) {
      if(dq[selected_vertex[sv]][j].first != sv) {
        Vertex dqv = dq[selected_vertex[sv]][j].first;
        size_t dqi = dq[selected_vertex[sv]][j].second;
        if(dag_candidate_getdegree[dqv][dqi] == dag_query_getdegree[dqv]) {
          full_indegree[dqv]++;
        }
      }
    }

    for(size_t j = 0; j < dag_query_edge[sv].size(); j++) {
      Vertex qv = dag_query_edge[sv][j];
      dag_query_getdegree[qv]--;
    }

    for(size_t j = 0; j < dag_candidate_edge[sv][i].size(); j++) {
      Vertex qv = dag_candidate_edge[sv][i][j].first;
      Vertex qvd = dag_candidate_edge[sv][i][j].second;
      if(!is_selected[cs.GetCandidate(qv, qvd)]) {
        dag_candidate_getdegree[qv][qvd]--;
      }
    }

    is_selected[selected_vertex[sv]] = false;

    selected_vertex[sv] = -1;
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

  only_v.resize(data.GetNumVertices(), -1);

  dq.resize(data.GetNumVertices());
  for(size_t i = 0; i < data.GetNumVertices(); i++) dq[i].clear();

  back_cnt.resize(qs, 0);


  // v -> u map
  for(size_t i = 0; i < qs; i++) {
    for(size_t ci = 0; ci < cs.GetCandidateSize(i); ci++) {
      Vertex j = cs.GetCandidate(i, ci);
      dq[j].push_back(std::make_pair(i, ci));
    }
  }


  // vertex that has only one candidate has super order
  for(size_t i = 0; i < qs; i++) if(cs.GetCandidateSize(i) == 1) {
    only_v[cs.GetCandidate(i, 0)] = i;
  }


  // make DAG with cs size order
  std::vector<std::pair<std::pair<size_t, size_t>, Vertex> > ord;
  ord.clear();
  for(size_t i = 0; i < qs; i++) ord.push_back(std::make_pair(std::make_pair(cs.GetCandidateSize(i), qs - query.GetDegree(i)), i));
  std::sort(ord.begin(), ord.end());

  for(size_t oi = 0; oi < qs; oi++) {
    Vertex i = ord[oi].second;
    visited[i] = true;
    for(size_t t = query.GetNeighborStartOffset(i); t < query.GetNeighborEndOffset(i); t++) {
      Vertex j = query.GetNeighbor(t);
      if(!visited[j]) {
        dag_query_edge[i].push_back(j);
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

  // initial for start vertex
  for(size_t i = 0; i < qs; i++) if(dag_query_indegree[i] == 0) {
    full_indegree[i] = cs.GetCandidateSize(i);
  }

  //run backtracking
  DoSelect(data, query, cs);

  //std::cout << ans_num << "\n";
  //if(ans_num == 100000) std::cout << "find 100000 matching!" << "\n";

  return;
}
