#include "Asap.h"
#include "bit_vectors.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <utility>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <sdsl/wavelet_trees.hpp>
#include <sdsl/vectors.hpp>

using namespace std;
using namespace sdsl;

struct RunStats {
    unsigned count;    // Cantidad de runs
    unsigned totalLength;    // Suma de las longitudes de los runs
};

// Función para contar los runs y calcular el largo promedio
RunStats countRuns(const sdsl::int_vector<>& vec) {
    RunStats stats;
    stats.count = 0;
    stats.totalLength = 0;

    int prevValue = vec[0];
    unsigned runLength = 1;

    for (size_t i = 1; i < vec.size(); ++i) {
        if (vec[i] != prevValue) {
            ++stats.count;
            stats.totalLength += runLength;
            prevValue = vec[i];
            runLength = 1;
        } else {
            ++runLength;
        }
    }

    return stats;
}



template<class BitVectorClass, class IntType, class WaveletClass>
Asap<BitVectorClass, IntType, WaveletClass>::Asap ( string input_file, unsigned method ) {
  IntType* s;
  // unordered_map<char, unsigned> freq;                        // freq[caracter] = frecuencia
  unordered_map<unsigned, unsigned> s_indexes;

  // Obtencion de las frequencias y de la cantidad de caracteres
  tie(s, text_length, freq) = Asap::readfile(input_file);
  // for (auto &x: freq)
  //   cout<< x.first << " " << x.second << endl;
  //cout << "text_length " << text_length << "\n";
  m = new M_WT<IntType>(freq, text_length, method);
  unordered_map<unsigned, int_vector<>> s_vectors;
  unordered_map<unsigned, bit_vector> temp_bit_vectors;
  for ( auto &x: m->m_sizes ) {
    temp_bit_vectors[x.first] = bit_vector(text_length+1, 0); // <---
    s_vectors[x.first] = int_vector<>(x.second+1);
  }

  // creacion de los bitvectors
  unsigned l;
  for ( unsigned i = 0; i < text_length; i++ ) {
    l = m->map(s[i]);
    temp_bit_vectors[l][i] = 1; // <---
    s_vectors[l][s_indexes[l]++] = m->rank(m->get_pos_by_char(s[i]), l);
    if(i+1 == text_length){
      s_vectors[l][s_indexes[l]++] = m->rank(m->get_pos_by_char(s[i]), l);
    }
  }

  for ( auto &x: s_vectors )
    if ( ! m->is_singleton(x.first) )
        construct_im(s_wt_trees[x.first], x.second, 0);


  // Obtener el nombre del archivo de salida
  std::string outputFileName = "base_path" + input_file + "_runs_si.txt";
    // Crear el archivo de salida
  std::ofstream outputFile(outputFileName);
  for (auto& tree : s_vectors) {        
    // Contar los runs y calcular el largo promedio
    RunStats treeStats = countRuns(tree.second);
    // Hacer algo con `treeStats.count` (cantidad de runs) y `treeStats.totalLength` (suma de las longitudes de los runs)
    // Calcular el largo promedio
    double avgLength = static_cast<double>(treeStats.totalLength) / treeStats.count;

    outputFile <<  treeStats.count << "," << avgLength<< std::endl;

  }
  // Cerrar el archivo de salida
  outputFile.close();

  //uint64_t contador = 0;
  //ofstream myfile ("/data/pizzachili/Resultados/BV_einstein_en.dat",ios::binary);
  //for ( auto &y: temp_bit_vectors ){
  //    contador+=1;
  //}

  //myfile << contador << "\n";
  //myfile.write( (char*)&contador, sizeof(uint64_t)); 
  //int breaka = 0;
  //for ( auto &x: temp_bit_vectors ){
      //cout << "size: " << x.second.size() << "\n";
      //cout << x.second << "\n"; 
      //uint64_t universo = x.second.size();
      //myfile.write( (char*)&universo , sizeof(uint64_t)); 
      //for (uint64_t i=0; i < x.second.size() ; i+=1){
          //if(x.second[i] == 1){
              //myfile << i << " ";
            //  myfile.write( (char*)&i , sizeof(uint64_t)); 
          //}
      //}
      //myfile << "\n";
  //}
  //myfile.close();

  for ( auto &x: temp_bit_vectors ){
    //cout << "x : " << x.first << "\n";
    //cout << "x : " << x.second.size() << "\n";
    //cout << "x : " << x.second.size() << " | ";
    bit_vectors.insert(std::make_pair<unsigned, BitVectorClass*>((unsigned int)x.first, new BitVectorClass(x.second)));
    //cout << "xn : " << x.second.size() << "\n";
    //cout << "\n\n";
  }

  //cout << "FINALIZADO";

}

template<class BitVectorClass, class IntType, class WaveletClass>
Asap<BitVectorClass, IntType, WaveletClass>::Asap ( string input_file, unsigned method, int flag ) {
  IntType* s;
  unordered_map<unsigned, unsigned> s_indexes;
  Uflag = flag;
  unordered_map<unsigned, int_vector<>> s_vectors;
  unordered_map<unsigned, bit_vector> temp_bit_vectors;

  
  if (flag==1){
    tie(s, text_length, freq) = Asap::readfile(input_file);
    uint64_t p = ceil(log2((double)text_length))/2;
    vector<uint8_t> t;
    vector<vector<uint64_t>> s_vector(p+1);
    for (uint64_t i = 0; i < text_length; i++) {
      
      t.push_back(floor(s[i] / ((ceil(freq.size() / p)))));
      s_vector[floor(s[i] / ((ceil(freq.size() / p))))].push_back(floor(s[i] % ((uint64_t)ceil(freq.size() / p))));
    }
    for (uint64_t i = 0; i < p+1; i++) {
      temp_bit_vectors[i]= bit_vector(text_length+1, 0);
      s_vectors[i] = int_vector<>(s_vector[i].size()+1);
    }

  // creacion de los bitvectors
  unsigned l;
  unsigned k;
  for ( unsigned i = 0; i < text_length; i++ ) {
    l = floor(s[i]/((ceil(freq.size()/p))));
    k = s[i]%((uint64_t)ceil(freq.size()/p));
    temp_bit_vectors[l][i] = 1; 
    s_vectors[l][s_indexes[l]++] = k;
    if(i+1 == text_length){
      s_vectors[l][s_indexes[l]++] = k;
    }
  }
  for ( auto &x: s_vectors )
    //if ( ! m->is_singleton(x.first) )
    construct_im(s_wt_trees[x.first], x.second, 0);


  // Obtener el nombre del archivo de salida
  std::string outputFileName = "base_path" + input_file + "_runs_si.txt";
    // Crear el archivo de salida
  std::ofstream outputFile(outputFileName);
  for (auto& tree : s_vectors) {        
    // Contar los runs y calcular el largo promedio
    RunStats treeStats = countRuns(tree.second);
    // Hacer algo con `treeStats.count` (cantidad de runs) y `treeStats.totalLength` (suma de las longitudes de los runs)
    // Calcular el largo promedio
    double avgLength = static_cast<double>(treeStats.totalLength) / treeStats.count;

    outputFile <<  treeStats.count << "," << avgLength<< std::endl;

  }
  // Cerrar el archivo de salida
  outputFile.close();

  for ( auto &x: temp_bit_vectors ){
  
    bit_vectors.insert(std::make_pair<unsigned, BitVectorClass*>((unsigned int)x.first, new BitVectorClass(x.second)));
  
  }
  }

  //cout << "FINALIZADO";
  else if (flag==2){
     tie(s, text_length, freq) = Asap::readfile(input_file);
    uint64_t p = ceil(log2((double)text_length))/2;
    vector<uint8_t> t;
    vector<vector<uint64_t>> s_vector(p+1);
    for (uint64_t i = 0; i < text_length; i++) {
      
      t.push_back(floor(s[i]%p));
      s_vector[floor(s[i]%p)].push_back(floor((double)s[i]/p));
    }
    for (uint64_t i = 0; i < p+1; i++) {
      temp_bit_vectors[i]= bit_vector(text_length+1, 0);
      s_vectors[i] = int_vector<>(s_vector[i].size()+1);
    }

  // creacion de los bitvectors
  unsigned l;
  unsigned k;
  for ( unsigned i = 0; i < text_length; i++ ) {
    l = floor(s[i]%p);
    k = floor((double)s[i]/p);
    temp_bit_vectors[l][i] = 1; 
    s_vectors[l][s_indexes[l]++] = k;
    if(i+1 == text_length){
      s_vectors[l][s_indexes[l]++] = k;
    }
  }
  for ( auto &x: s_vectors )
    //if ( ! m->is_singleton(x.first) )
        construct_im(s_wt_trees[x.first], x.second, 0);


  // Obtener el nombre del archivo de salida
  std::string outputFileName = "base_path" + input_file + "_runs_si.txt";
    // Crear el archivo de salida
  std::ofstream outputFile(outputFileName);
  for (auto& tree : s_vectors) {        
    // Contar los runs y calcular el largo promedio
    RunStats treeStats = countRuns(tree.second);
    // Hacer algo con `treeStats.count` (cantidad de runs) y `treeStats.totalLength` (suma de las longitudes de los runs)
    // Calcular el largo promedio
    double avgLength = static_cast<double>(treeStats.totalLength) / treeStats.count;

    outputFile <<  treeStats.count << "," << avgLength<< std::endl;

  }
  // Cerrar el archivo de salida
  outputFile.close();

  for ( auto &x: temp_bit_vectors ){
  
    bit_vectors.insert(std::make_pair<unsigned, BitVectorClass*>((unsigned int)x.first, new BitVectorClass(x.second)));
  
  }
  }
  else if (flag==3){
    unsigned new_length;
    tie(s,new_length,text_length,freq,fullfreq) = Asap::readfileFlag(input_file,flag);
    m = new M_WT<IntType>(freq,fullfreq, new_length, method);


    // uint64_t p = ceil(log2((double)text_length));
    // vector<vector<uint64_t>> s_vector(p);
    // cout<<">>p: "<<p<<endl;
    // for (uint64_t i = 0; i < text_length; i++) {
    //        //s_vector[m->map(s[i])].push_back(m->rank(m->get_pos_by_char(s[i]), m->map(s[i])));
    //        cout<<m->rank(m->get_pos_by_char(s[i]), m->map(s[i]))<<endl;
    //    }
    // for (uint64_t i = 0; i < p+1; i++) {
    //   temp_bit_vectors[i]= bit_vector(text_length+1, 0);
    //   s_vectors[i] = int_vector<>(s_vector[i].size()+1);
    // }
    for ( auto &x: m->m_sizes ) {
    temp_bit_vectors[x.first] = bit_vector(text_length+1, 0); // <---
    s_vectors[x.first] = int_vector<>(x.second+1);
    //cout<<"x_first"<<x.first<<"<<x_second "<<x.second<<endl;
    }
  //creacion de los bitvectors
  unsigned count=0;
  unsigned l;
  unsigned k;
  for ( unsigned i = 0; i < text_length; i++ ) {
    count+=1;
    //cout<<1<<endl;
    l = m->map(s[i]);
    temp_bit_vectors[l][i] = 1;
    //cout<<4<<endl;
    //cout<<"m->get_pos_by_char("<<s[i]<<"): "<<k<<endl;
    s_vectors[l][s_indexes[l]++]=m->rank(m->get_pos_by_char(s[i]), l); //Esto puede fallar 
  
    //cout<<5<<endl;
    //cout<<"count"<<count<<endl;
    if(i+1 == text_length){
      s_vectors[l][s_indexes[l]++] = m->rank(m->get_pos_by_char(s[i]), l);
    }
  }

  for ( auto &x: s_vectors )
    if ( ! m->is_singleton(x.first) )
        construct_im(s_wt_trees[x.first], x.second, 0);

  // Obtener el nombre del archivo de salida
  std::string outputFileName = "base_path" + input_file + "_runs_si.txt";
    // Crear el archivo de salida
  std::ofstream outputFile(outputFileName);
  for (auto& tree : s_vectors) {        
    // Contar los runs y calcular el largo promedio
    RunStats treeStats = countRuns(tree.second);
    // Hacer algo con `treeStats.count` (cantidad de runs) y `treeStats.totalLength` (suma de las longitudes de los runs)
    // Calcular el largo promedio
    double avgLength = static_cast<double>(treeStats.totalLength) / treeStats.count;

    outputFile <<  treeStats.count << "," << avgLength<< std::endl;

  }
  // Cerrar el archivo de salida
  outputFile.close();

  for ( auto &x: temp_bit_vectors ){
  
    bit_vectors.insert(std::make_pair<unsigned, BitVectorClass*>((unsigned int)x.first, new BitVectorClass(x.second)));
  }
  }
}
template<class BitVectorClass, class IntType, class WaveletClass>
Asap<BitVectorClass, IntType, WaveletClass>::~Asap ( void ) {
}


template<class BitVectorClass, class IntType, class WaveletClass>
unsigned long Asap<BitVectorClass, IntType, WaveletClass>::textsize () {
  return this->text_length;
}

template<class BitVectorClass, class IntType, class WaveletClass>
IntType Asap<BitVectorClass, IntType, WaveletClass>::access ( unsigned position ) {
  unsigned l = 0;
  for ( auto &x: bit_vectors )
    if ( (x.second)->access(position) == 1 ) {
      l = x.first;
      break;
    }

  if ( m->is_singleton(l) ) return m->get_char_by_pos(m->select(1, l));

  unsigned k = (bit_vectors[l])->rank(position);
  return m->get_char_by_pos(m->select(s_wt_trees[l][k] + 1, l));
}

// template<class BitVectorClass, class IntType, class WaveletClass>
// unsigned Asap<BitVectorClass, IntType,  WaveletClass>::rank ( unsigned position , IntType target ) {
//   //cout << "r : ";

//   int l = m->map(target);
//   //cout << "l:" << l  <<" ";
//   // unsigned k = (bit_vectors[l])->rank(position - 1);

//   //cout << "Largo de la secuencia: " << (bit_vectors[l])->size() << endl;
//   unsigned k = (bit_vectors[l])->rank(position);
//   //cout << "k:" << k  << endl;

// //unsigned k = (bit_vectors[l])->rank(position+1);
//   if ( m->is_singleton(l) ) return k;

//   unsigned c = m->rank(m->get_pos_by_char(target), l);
//   //cout << "c_:" << c << endl;

//   //cout << "s_wt:" << s_wt_trees[l].rank(k, c) << endl;
//   return s_wt_trees[l].rank(k, c);
// }

template<class BitVectorClass, class IntType, class WaveletClass>
unsigned Asap<BitVectorClass, IntType,  WaveletClass>::rank ( unsigned position , IntType target ) {
  int l;
  unsigned n;
  if (Uflag==1){
    uint64_t p = ceil(log2((double)text_length))/2;
    l = floor(target/((ceil(freq.size()/p))));
    n = target%((uint64_t)ceil(freq.size()/p));
  }
  else if (Uflag==2){
    uint64_t p = ceil(log2((double)text_length))/2;
    l = floor(target%p);
    n = floor((double)target/p);
  }
  else{
    l = m->map(target);
    n = m->rank(m->get_pos_by_char(target), l);
    unsigned k = (bit_vectors[l])->rank(position);
    if ( m->is_singleton(l) ) return k;
    unsigned c = m->rank(m->get_pos_by_char(target), l);
    return s_wt_trees[l].rank(k, n);
  }
  unsigned k = (bit_vectors[l])->rank(position);
  //if ( m->is_singleton(l) ) return k;

  //unsigned c = m->rank(m->get_pos_by_char(target), l);
  return s_wt_trees[l].rank(k, n);
}

template<class BitVectorClass, class IntType, class WaveletClass>
int Asap<BitVectorClass, IntType, WaveletClass>::select ( unsigned position , IntType target) {
  int l = m->map(target);
  if ( l == -1 ) return -1;

  if ( m->is_singleton(l) ) return (bit_vectors[l])->select(index);

  unsigned c = m->rank(m->get_pos_by_char(target), l);
  unsigned k = s_wt_trees[l].select(index, c) + 1;
  return (bit_vectors[l])->select(k);
}

template<class BitVectorClass, class IntType, class WaveletClass>
IntType* Asap<BitVectorClass, IntType, WaveletClass>::waccess ( unsigned start, unsigned end ) {
  if ( end < start ) {
    unsigned temp = start;
    start = end;
    end = temp;
  }
  end = end < text_length ? end : text_length - 1;
  IntType* response = new IntType[end - start + 1];
  unsigned length = end - start + 1;
  unsigned relevant_bits, before_start_rank, end_rank;
  for ( auto &x: bit_vectors ) {
    before_start_rank = start == 0 ? 0 : (x.second)->rank(start);
    end_rank = (x.second)->rank(end+1);
    relevant_bits = end_rank - before_start_rank;

    if ( relevant_bits > 0 ) {
      unsigned nextOne, l = x.first, k = before_start_rank;
      for ( unsigned i = 1; nextOne = (x.second)->select(before_start_rank + i), nextOne <= end && relevant_bits > 0 ; i++, relevant_bits-- ) {
        response[nextOne - start] = m->get_char_by_pos(m->select( (m->is_singleton(l) ? 0 : s_wt_trees[l][k + 1]), l));
        //response[nextOne - start] = m->get_char_by_pos(m->select( (m->is_singleton(l) ? 0 : s_wt_trees[l][k]) + 1, l));
        length--;
        k += 1;
      }
    }
    if ( length == 0 ) break;
  }

  return response;
}

template<class BitVectorClass, class IntType, class WaveletClass>
unsigned long Asap<BitVectorClass, IntType, WaveletClass>::size () {
  unsigned long sum=0;
  if(Uflag==3){
    sum = m->size();
  }
  for ( auto &x: s_wt_trees )
    if ( x.second.sigma > 1 )
      sum += size_in_bytes(x.second);
  for ( auto &x: bit_vectors )
    sum += (x.second)->size();
  return sum;

  /*unsigned long sum = m->size();
  for ( auto &x: s_wt_trees )
    if ( x.second.sigma > 1 )
      sum += size_in_bytes(x.second);
  for ( auto &x: bit_vectors )
    sum += size_in_bytes(x.second);
  return sum;*/
}
template<class BitVectorClass, class IntType, class WaveletClass>
unsigned long Asap<BitVectorClass, IntType, WaveletClass>::size2 () {
  unsigned long sum = m->size();
  for ( auto &x: s_wt_trees )
    if ( x.second.sigma > 1 )
      sum += size_in_bytes(x.second);
  for ( auto &x: bit_vectors )
    sum += (x.second->size_in_bits_formula()/8);
  return sum;
}

template<class BitVectorClass, class IntType, class WaveletClass>
unsigned long Asap<BitVectorClass, IntType, WaveletClass>::m_size () {
  return m->size();
}

template<class BitVectorClass, class IntType, class WaveletClass>
unsigned Asap<BitVectorClass, IntType, WaveletClass>::partitions () {
  return m->partitions();
}

template<class BitVectorClass, class IntType, class WaveletClass>
unsigned long Asap<BitVectorClass, IntType, WaveletClass>::b_size () {
  unsigned long sum = 0;
  for ( auto &x: bit_vectors )
    sum += (x.second)->size();
  return sum;
}

template<class BitVectorClass, class IntType, class WaveletClass>
unsigned long Asap<BitVectorClass, IntType, WaveletClass>::sl_size () {
  unsigned long sum = 0;
  for ( auto &x: s_wt_trees )
    if ( x.second.sigma > 1 )
      sum += size_in_bytes(x.second);
  return sum;
}

template<class BitVectorClass, class IntType, class WaveletClass>
tuple<IntType*, unsigned, unordered_map<IntType, unsigned>> Asap<BitVectorClass, IntType, WaveletClass>::readfile ( string input_file ) {
  FILE *file;
  IntType* buffer;
  unsigned length, result;
  unordered_map<IntType, unsigned> freq;

  file = fopen(input_file.c_str(), "rb");
  fseek(file, 0, SEEK_END);
  length = ftell(file)/sizeof(IntType);
  rewind(file);
  buffer = new IntType[length];
  result = fread(buffer, sizeof(IntType), length, file);

  if ( result != length ) { cout << "fread error" << endl; exit(3);}

  fclose(file);

  for ( unsigned i = 0; i < length; i++ )
    freq[buffer[i]]++;

  return make_tuple (buffer, length, freq);
}

template<class BitVectorClass, class IntType, class WaveletClass>
tuple<IntType*, unsigned,unsigned, unordered_map<IntType, unsigned>,unordered_map<IntType, unsigned>> Asap<BitVectorClass, IntType, WaveletClass>::readfileFlag ( string input_file, int flag ) {
  FILE *file;
  IntType* buffer;
  unsigned length, result;
  unordered_map<IntType, unsigned> freq;
  unordered_map<IntType, unsigned> fullfreq;
  file = fopen(input_file.c_str(), "rb");
  fseek(file, 0, SEEK_END);
  length = ftell(file)/sizeof(IntType);
  rewind(file);
  buffer = new IntType[length];
  result = fread(buffer, sizeof(IntType), length, file);

  if ( result != length ) { cout << "fread error" << endl; exit(3);}

  fclose(file);
  for ( unsigned i = 0; i < length; i++ )
    fullfreq[buffer[i]]++; 
  vector<uint64_t> v_aux;
  for (uint64_t i = 1; i < length; i++) {
    if (buffer[i] != buffer[i-1])
      v_aux.push_back(buffer[i-1]);
  }    IntType* new_buffer;
  new_buffer = new IntType[v_aux.size()];
  for (size_t i = 0; i < v_aux.size(); i++) {
    new_buffer[i] = v_aux[i];
  }
    
  for ( unsigned i = 0; i < v_aux.size(); i++ )
    freq[new_buffer[i]]++;
  return make_tuple (buffer, v_aux.size(),length, freq,fullfreq);
  
  
}

template<class BitVectorClass, class IntType, class WaveletClass>
unsigned Asap<BitVectorClass, IntType, WaveletClass>::length () {
  return text_length;
}

template<class BitVectorClass, class IntType, class WaveletClass>
unsigned Asap<BitVectorClass, IntType, WaveletClass>::alphabet_size() {
  return freq.size();
}

template<class BitVectorClass, class IntType, class WaveletClass>
std::unordered_map<IntType, unsigned> Asap<BitVectorClass, IntType, WaveletClass>::frequency() {
  return freq;
}

template<class BitVectorClass, class IntType, class WaveletClass>
vector<IntType> Asap<BitVectorClass, IntType, WaveletClass>::alphabet () {
  vector<IntType> alphabet;
  for ( auto &x: freq )
    alphabet.push_back(x.first);
  return alphabet;
}

template<class BitVectorClass, class IntType, class WaveletClass>
std::tuple<IntType, double, bool, double, double, double, double> Asap<BitVectorClass, IntType, WaveletClass>::access_timecheck ( unsigned position ) {
  double l_find_t, singleton_t, bv_rank_t, wt_access_t, m_t;
  clock_t begin, end;

  begin = clock();
  unsigned l;
  for ( auto &x: bit_vectors )
    if ( (x.second)->access(position) == 1 ) {
      l = x.first;
      break;
    }
  end = clock();
  l_find_t = double(end-begin) / CLOCKS_PER_SEC;

  begin = clock();
  if ( m->is_singleton(l) ) {
    begin = clock();
    IntType answer = m->get_char_by_pos(m->select(1, l));
    end = clock();
    m_t = double(end-begin) / CLOCKS_PER_SEC;

    return make_tuple(answer, l_find_t, true, 0, 0, 0, m_t);
  }
  end = clock();
  singleton_t = double(end-begin) / CLOCKS_PER_SEC;

  begin = clock();
  unsigned k = (bit_vectors[l])->rank(position);
  end = clock();
  bv_rank_t = double(end-begin) / CLOCKS_PER_SEC;

  begin = clock();
  auto temp = s_wt_trees[l][k];
  end = clock();
  wt_access_t = double(end-begin) / CLOCKS_PER_SEC;

  begin = clock();
  IntType temp2 = m->get_char_by_pos(m->select(temp + 1, l));
  end = clock();
  m_t = double(end-begin) / CLOCKS_PER_SEC;
  return make_tuple(temp2, l_find_t, false, singleton_t, bv_rank_t, wt_access_t, m_t);
}

template<class BitVectorClass, class IntType, class WaveletClass>
std::tuple<unsigned, double, double, bool, double, double, double> Asap<BitVectorClass, IntType, WaveletClass>::rank_timecheck ( IntType target, unsigned position ) {
  double l_find_t, bv_rank_t, singleton_t, m_rank_t, wt_rank_t;
  clock_t begin, end;

  begin = clock();
  int l = m->map(target);
  end = clock();
  l_find_t = double(end-begin) / CLOCKS_PER_SEC;

  begin = clock();
  unsigned k = (bit_vectors[l])->rank(position + 1);
  end = clock();
  bv_rank_t = double(end-begin) / CLOCKS_PER_SEC;

  begin = clock();
  if ( m->is_singleton(l) ) {
    return make_tuple(k, l_find_t, bv_rank_t, true, 0, 0, 0);
  }
  end = clock();
  singleton_t = double(end-begin) / CLOCKS_PER_SEC;

  begin = clock();
  unsigned c = m->rank(m->get_pos_by_char(target), l);
  end = clock();
  m_rank_t = double(end-begin) / CLOCKS_PER_SEC;

  begin = clock();
  unsigned temp = s_wt_trees[l].rank(k, c);
  end = clock();
  wt_rank_t = double(end-begin) / CLOCKS_PER_SEC;

  return make_tuple(temp, l_find_t, bv_rank_t, false, singleton_t, m_rank_t, wt_rank_t);
}

template<class BitVectorClass, class IntType, class WaveletClass>
std::tuple<int, double, bool, double, double, double, double> Asap<BitVectorClass, IntType, WaveletClass>::select_timecheck ( IntType target, unsigned index) {
  double find_l_t, singleton_t, m_rank_t, wt_select_t, bv_select_t;
  clock_t begin, end;

  begin = clock();
  int l = m->map(target);
  end = clock();
  find_l_t = double(end-begin) / CLOCKS_PER_SEC;

  begin = clock();
  if ( m->is_singleton(l) ) {
    begin = clock();
    int answer = (bit_vectors[l])->select(index);
    end = clock();
    bv_select_t = double(end-begin) / CLOCKS_PER_SEC;
    return make_tuple(answer, find_l_t, true, 0, 0, 0, bv_select_t);
  }
  end = clock();
  singleton_t = double(end-begin) / CLOCKS_PER_SEC;

  begin = clock();
  unsigned c = m->rank(m->get_pos_by_char(target), l);
  end = clock();
  m_rank_t = double(end-begin) / CLOCKS_PER_SEC;

  begin = clock();
  unsigned k = s_wt_trees[l].select(index, c) + 1;
  end = clock();
  wt_select_t = double(end-begin) / CLOCKS_PER_SEC;

  begin = clock();
  int temp = (bit_vectors[l])->select(k);
  end = clock();
  bv_select_t = double(end-begin) / CLOCKS_PER_SEC;

  return make_tuple(temp, find_l_t, false, singleton_t, m_rank_t, wt_select_t, bv_select_t);
}


template<class BitVectorClass, class IntType, class WaveletClass>
std::tuple<IntType, bool, unsigned, double, double> Asap<BitVectorClass, IntType, WaveletClass>::accessTime ( unsigned position ) {
  IntType answer;
  clock_t begin = clock(), end;
  unsigned l = 0;
  for ( auto &x: bit_vectors )
    if ( (x.second)->access(position) == 1 ) {
      l = x.first;
      break;
    }
  end = clock();

  double b_and_s_time = double(end-begin) / CLOCKS_PER_SEC, m_time;

  begin = clock();
  if ( m->is_singleton(l) ) {
    answer = m->get_char_by_pos(m->select(1, l));
    end = clock();

    m_time = double(end-begin) / CLOCKS_PER_SEC;
    return std::make_tuple(answer, true, l, m_time, b_and_s_time);
  }
  end = clock();

  m_time = double(end-begin) / CLOCKS_PER_SEC;

  begin = clock();
  unsigned k = (bit_vectors[l])->rank(position);
  end = clock();

  b_and_s_time += double(end-begin) / CLOCKS_PER_SEC;

  begin = clock();
  answer = m->get_char_by_pos(m->select(s_wt_trees[l][k] + 1, l));
  end = clock();

  m_time += double(end-begin) / CLOCKS_PER_SEC;
  return std::make_tuple(answer, true, l, m_time, b_and_s_time);
}

template<class BitVectorClass, class IntType, class WaveletClass>
std::tuple<unsigned, bool, unsigned, double, double> Asap<BitVectorClass, IntType, WaveletClass>::rankTime ( IntType target, unsigned position ) {
  unsigned answer;
  clock_t begin = clock(), end;
  int l = m->map(target);
  end = clock();

  double m_time = double(end-begin) / CLOCKS_PER_SEC;

  begin = clock();
  unsigned k = (bit_vectors[l])->rank(position + 1);
  end = clock();

  double b_and_s_time = double(end-begin) / CLOCKS_PER_SEC;

  begin = clock();
  if ( m->is_singleton(l) ) {
    answer = k;
    end = clock();

    m_time += double(end-begin) / CLOCKS_PER_SEC;
    return std::make_tuple(answer, true, l, m_time, b_and_s_time);
  }

  unsigned c = m->rank(m->get_pos_by_char(target), l);
  end = clock();

  m_time += double(end-begin) / CLOCKS_PER_SEC;

  begin = clock();
  answer = s_wt_trees[l].rank(k, c);
  end = clock();

  b_and_s_time += double(end-begin) / CLOCKS_PER_SEC;
  return std::make_tuple(answer, true, l, m_time, b_and_s_time);
}

template<class BitVectorClass, class IntType, class WaveletClass>
std::tuple<int, bool, unsigned, double, double> Asap<BitVectorClass, IntType, WaveletClass>::selectTime ( IntType target, unsigned index) {
  unsigned answer;
  double m_time, b_and_s_time;
  clock_t begin = clock(), end;
  int l = m->map(target);
  if ( l == -1 ) {
    return std::make_tuple(-1, false, -1, 0, 0);
  }

  if ( m->is_singleton(l) ) {
    end = clock();
    m_time = double(end-begin) / CLOCKS_PER_SEC;

    begin = clock();
    answer = (bit_vectors[l])->select(index);
    end = clock();

    b_and_s_time = double(end-begin) / CLOCKS_PER_SEC;
    return std::make_tuple(answer, true, l, m_time, b_and_s_time);
  }

  unsigned c = m->rank(m->get_pos_by_char(target), l);
  end = clock();

  m_time = double(end-begin) / CLOCKS_PER_SEC;

  begin = clock();
  unsigned k = s_wt_trees[l].select(index, c) + 1;
  answer = (bit_vectors[l])->select(k);
  end = clock();

  b_and_s_time = double(end-begin) / CLOCKS_PER_SEC;
  return std::make_tuple(answer, true, l, m_time, b_and_s_time);
}

template<class BitVectorClass, class IntType, class WaveletClass>
std::tuple<IntType*, unordered_map<unsigned, double>> Asap<BitVectorClass, IntType, WaveletClass>::waccessTime ( unsigned start, unsigned end ) {
  double initialTime;
  unordered_map<unsigned, double> partitionTime;
  clock_t begin = clock(), clockEnd;

  if ( end < start ) {
    unsigned temp = start;
    start = end;
    end = temp;
  }
  end = end < text_length ? end : text_length - 1;
  IntType* response = new IntType[end - start + 1];
  unsigned length = end - start + 1;
  unsigned relevant_bits, before_start_rank, end_rank;

  clockEnd = clock();

  initialTime = double(clockEnd-begin) / CLOCKS_PER_SEC;

  for ( auto &x: bit_vectors ) {
    begin = clock();

    before_start_rank = start == 0 ? 0 : (x.second)->rank(start);
    end_rank = (x.second)->rank(end+1);
    relevant_bits = end_rank - before_start_rank;

    if ( relevant_bits > 0 ) {
      unsigned nextOne, l = x.first, k = before_start_rank;
      for ( unsigned i = 1; nextOne = (x.second)->select(before_start_rank + i), nextOne <= end && relevant_bits > 0 ; i++, relevant_bits-- ) {
        response[nextOne - start] = m->get_char_by_pos(m->select( (m->is_singleton(l) ? 0 : s_wt_trees[l][k]) + 1, l));
        length--;
        k += 1;
      }
    }

    clockEnd = clock();

    partitionTime[x.first] = double(clockEnd-begin) / CLOCKS_PER_SEC;
    if ( length == 0 ) break;
  }

  for ( auto &x: partitionTime )
    x.second += initialTime;

  return std::make_tuple(response, partitionTime);
}
