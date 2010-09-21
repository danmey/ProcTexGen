#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <iterator>
#include <string.h>
#include <stdlib.h>
#include <deque>
#include <string>
#include "perlin.h"
//#include "tga.h"
#include "bmp.h"

using namespace std;
 
bool is_one_of(int ch, char* chars) {
  int size = strlen(chars);
  for(int i=0; i<size; i++) if ( ch == chars[i] ) return true;
  return false;
}

int split_string(const std::string& in_string, char* delimit, std::string split[], int max_split)
{
  int split_index=0;
  for(int i=0; i<in_string.size(); i++) {
    if ( is_one_of(in_string[i], delimit)) { 
      i++; split_index++; 
      if ( split_index > max_split ) return max_split; 
      split[split_index] = ""; 
    }
    split[split_index] += in_string[i];
  }
  //cout << "split: " << split[split_index].c_str() << " " << split_index << "\n";
  return split_index;
}

bool is_sexp_sep(int ch) { return is_one_of(ch, "()"); }

void skip_whites(istream& stream)
{
  int ch;
  while((ch = stream.peek()) != EOF && isspace(ch) ) 
    stream.get();
}

int get_skipwhites(istream& stream)
{
  skip_whites(stream);
  return stream.get();
}

bool get_symbol(istream& stream, std::string& token) {
  token="";
  int ch;
  while((ch = stream.peek()) != EOF && !isspace(ch) && !is_sexp_sep(ch) ) {
    token+= (char)ch;
    stream.get();
  }
  return stream.peek() != EOF;
  
}

bool get_token(istream& stream, std::string& token) {
  token="";
  int ch = get_skipwhites(stream);

  if ( ch == EOF ) return false;

  if ( is_sexp_sep(ch) )
    token += (char)ch;
  else {
    stream.putback(ch); return get_symbol(stream, token); 
  }
  return true;
}

bool is_token_begin(std::string& token) { return token.size()==1 && token[0] == '('; }

bool is_token_end(std::string& token) { return token.size()==1 && token[0] == ')'; }

bool is_token_number_prefix(std::string& token) { 
  if ( token.size() > 1 && (is_one_of(token[0], ".+-") && isdigit(token[1])) ) return true;
  return false;
}

bool is_token_number(std::string& token) { 
  char* end;
  const char* start = token.c_str();
  //cout <<
  strtod(start, &end);
  return end-start == token.size();
}

bool is_token_symbol(std::string& token) {
  if (!is_token_begin(token) && !is_token_end(token) && !is_token_number(token) ) return true;
  return false;
}

void test_token(std::string& token) {
  if ( is_token_begin(token) ) cout << "Token:Begin ";
  if ( is_token_end(token) ) cout << "Token:End ";
  if ( is_token_symbol(token) ) cout << "Token:Symbol ";
  if ( is_token_number(token) ) cout << "Token:Number ";
}

class AST_Node {
public:
  typedef deque<AST_Node*>::iterator iterator;
  AST_Node(const std::string& value): m_value(value) {}
  AST_Node() {}
  std::string Value() const { return m_value; }
  bool IsList() { return Value().size() == 0; }
  iterator begin() { return m_terms.begin(); }
  iterator end() { return m_terms.end(); }
  void AddTerm(AST_Node* node) {  m_terms.push_back(node); }
private:
  std::string m_value;
  deque<AST_Node*> m_terms;
};

AST_Node* build_sexp(istream& stream)
{
  AST_Node* node = new AST_Node();
  string token;
  while ( get_token(stream, token ) && !is_token_end(token) )
    {
      if ( is_token_begin(token) ) 
        node->AddTerm(build_sexp(stream));
      else 
        node->AddTerm(new AST_Node(token));
    }
  return node;
}

vector<AST_Node*> build_AST(istream& stream)
{
  vector<AST_Node*> sexps;
  string token;
  while( get_token(stream, token ) && is_token_begin(token) && token != "!" ) 
    sexps.push_back(build_sexp(stream));
  return sexps;
}
 
static void tab_stream(ostream& str, int level)
{
  for(int i=0; i<level; i++)
    str << "\t";
}

void print_AST_node(ostream& stream, AST_Node* node, int nesting_level)
{
  //  static int nesting_level = 0;
  tab_stream(stream, nesting_level);

  if ( node->IsList() )
    cout << "Sexp\n";
  else 
    cout << node->Value().c_str() << "\n";

  nesting_level++;
  for(AST_Node::iterator it = node->begin(); it != node->end(); it++)
    print_AST_node(stream, *it, nesting_level);
  nesting_level--;
}
/*
struct op_param_t {
  static op_param_t* Float(const std::string& name, float default_val, int index) {
    op_param_t* p = new op_param_t(); p->name = name; p->default_val.f = default_val; 
    p->index = index; p->real = true; return p; }

  static op_param_t* Int(const std::string& name, int default_val, int index) {
    op_param_t* p = new op_param_t(); p->name = name; p->default_val.i = default_val; 
    p->index = index; p->real = false; return p; }
  op_param_t() {}
    
  std::string name;
  param default_val;
  int index;
  bool real;
};



struct gen_op_desc_t {
  std::string op_name;
  op::gen_func func;
  int n_o_layers; // == -1 the same as number of input layers
  
  op_param_t* 
};
*/
class TextureGeneration {
public:
  vector<layer> EvalExpression(map<string, vector<layer> >& evaluated_variables, AST_Node* s_exp);
  vector<layer> DefineOneVariable(map<string, vector<layer> >& evaluated_variables, AST_Node* var_def, string& var_name);
  vector<layer> Generate(vector<AST_Node*> tex_def);
  TextureGeneration();
				  
private:
  struct Param {
    static Param* Float(const std::string& name, float default_val, int index) {
      Param* p = new Param(); p->name = name; p->default_val.f = default_val; 
      p->index = index; p->real = true; return p; }

    static Param* Int(const std::string& name, int default_val, int index) {
      Param* p = new Param(); p->name = name; p->default_val.i = default_val; 
      p->index = index; p->real = false; return p; }
    Param() {}
    
    std::string name;
    param default_val;
    int index;
    bool real;
  };
  bool OrderedParameters(string& op_name, const vector<string>& names, const vector<double>& values, 
			 vector<param>& ordered_params);
  vector<layer> OperationResult(AST_Node* def, 
				const vector<layer>& input_layers, 
				const vector<float>& paramaters);
  map<string, AST_Node*> m_variables;
  map<AST_Node*, vector<layer> > m_cached_layers;
  map<std::string, map<std::string, Param*> > m_defined_parameters;
  map<std::string, int> m_n_out_layers;
};

inline op::gen_func get_operator(const char* name)
{
  if ( strcmp(name, "flat") == 0) return op::gen_flat;
  else if ( strcmp(name, "glow") == 0) return op::gen_glow;
  else if ( strcmp(name, "distort") == 0) return op::gen_distort;
  else if ( strcmp(name, "perlin") == 0) return op::gen_perlin;
  else if ( strcmp(name, "hsv") == 0) return op::gen_apply_HSV_const;
  else if ( strcmp(name, "rgb") == 0) return op::gen_rgb;
  else if ( strcmp(name, "+") == 0) return op::gen_add;
  else if ( strcmp(name, "*") == 0) return op::gen_mul;
  else if ( strcmp(name, "|") == 0) return op::gen_clamp;
  else if ( strcmp(name, "|<") == 0) return op::gen_normalize;
  else if ( strcmp(name, "+3") == 0) return op::gen_add;
  else if ( strcmp(name, "*3") == 0) return op::gen_mul;
  else if ( strcmp(name, "|3") == 0) return op::gen_clamp;
  else if ( strcmp(name, "|<3") == 0) return op::gen_normalize;
  else if ( strcmp(name, "pix") == 0) return op::gen_pixels;
  else if ( strcmp(name, "blur") == 0) return op::gen_blur;
  else if ( strcmp(name, "light") == 0) return op::gen_light;
  else if ( strcmp(name, "trans") == 0) return op::gen_transform;
  else if ( strcmp(name, "mix") == 0) return op::gen_mix;
  else if ( strcmp(name, "msk") == 0) return op::gen_mask;  
  else if ( strcmp(name, "alpha") == 0) return op::gen_alpha;
  else if ( strcmp(name, "alpha3") == 0) return op::gen_alpha;  
  else if ( strcmp(name, "brick") == 0) return op::gen_brick;  
  else NULL;
}

inline std::string get_operator_name(op::gen_func func)
{
  if ( func == op::gen_flat ) return std::string("flat");
  if ( func == op::gen_glow ) return std::string("glow");
  if ( func == op::gen_distort ) return std::string("distort");
  if ( func == op::gen_perlin ) return std::string("perlin");
  if ( func == op::gen_apply_HSV_const ) return std::string("hsv");
  if ( func == op::gen_rgb ) return std::string("rgb");
  if ( func == op::gen_add ) return std::string("+");
  if ( func == op::gen_mul ) return std::string("*");
  if ( func == op::gen_clamp ) return std::string("|");
  if ( func == op::gen_normalize ) return std::string("|<");
  if ( func == op::gen_add ) return std::string("+3");
  if ( func == op::gen_mul ) return std::string("*3");
  if ( func == op::gen_clamp ) return std::string("|3");
  if ( func == op::gen_normalize ) return std::string("|<3");
  if ( func == op::gen_pixels ) return std::string("pix");
  if ( func == op::gen_blur ) return std::string("blur");
  if ( func == op::gen_transform ) return std::string("trans");
  if ( func == op::gen_mix ) return std::string("mix");
  if ( func == op::gen_mask ) return std::string("msk");
  if ( func == op::gen_alpha ) return std::string("alpha");
  if ( func == op::gen_alpha ) return std::string("alpha3");    
  if ( func == op::gen_brick ) return std::string("brick");
}

TextureGeneration::TextureGeneration()
{
#define PARAM_START(op_name_)						\
  m_defined_parameters.insert( pair<std::string, map<std::string, Param*> >(op_name_, map<std::string, Param*>()) );
#define PARAM_INT(op_name_, param_name_, index_, default_val_)		\
  m_defined_parameters[op_name_].insert(pair<std::string, Param*>(param_name_, TextureGeneration::Param::Int(param_name_, default_val_, index_))); \
  //{char buf[32]; sprintf(buf, "%d", index_); /*cout << buf << "\n"*/; \
  //m_defined_parameters[op_name_].insert(pair<std::string, Param*>(buf, TextureGeneration::Param::Int(param_name_, default_val_, index_)));} 
#define PARAM_FLOAT(op_name_, param_name_, index_, default_val_)	\
  m_defined_parameters[op_name_].insert(pair<std::string, Param*>(param_name_, TextureGeneration::Param::Float(param_name_, default_val_, index_))); \
  //{char buf[32]; sprintf(buf, "%d", index_); /*cout << buf << "\n"*/; \
  //m_defined_parameters[op_name_].insert(pair<std::string, Param*>(buf, TextureGeneration::Param::Float(param_name_, default_val_, index_))); } 

  PARAM_START("perlin")
    //  m_defined_parameters["perlin"].insert(pair<std::string, Param*>("perlin", TextureGeneration::Param::Int("bla", 0, 0))); 
    PARAM_INT("perlin", "octaves", 0, 6);
  PARAM_FLOAT("perlin", "persistence", 1, 0.7);
  PARAM_INT("perlin", "m", 2, 1)
  m_n_out_layers["perlin"] = 1;

  PARAM_START("flat")
    PARAM_INT("flat", "x", 0, 0)
    PARAM_INT("flat", "y", 1, 0)
    PARAM_INT("flat", "w", 2, 256)
    PARAM_INT("flat", "h", 3, 256)
    PARAM_FLOAT("flat", "c", 4, 1)
    m_n_out_layers["flat"] = 1;

  PARAM_START("glow")
    PARAM_FLOAT("glow", "x", 0, 128)
    PARAM_FLOAT("glow", "y", 1, 128)
    PARAM_FLOAT("glow", "atten", 2, 5);
    PARAM_FLOAT("glow", "xr", 3, 1)
    PARAM_FLOAT("glow", "yr", 4, 1)
  m_n_out_layers["glow"] = 1;

  PARAM_START("pix");
  PARAM_INT("pix","n", 0, 10000);
  m_n_out_layers["pix"] = 1;

  PARAM_START("distort")
    PARAM_INT("distort", "type", 0, 0);
  m_n_out_layers["distort"] = 1;

  PARAM_START("hsv")
    PARAM_FLOAT("hsv","h", 0, 1)
    PARAM_FLOAT("hsv","s", 1, 1)
    PARAM_FLOAT("hsv","v", 2, 1)
    m_n_out_layers["hsv"] = 3;

  PARAM_START("rgb")
    PARAM_FLOAT("rgb","r", 0, 1)
    PARAM_FLOAT("rgb","g", 1, 1)
    PARAM_FLOAT("rgb","b", 2, 1)
    m_n_out_layers["rgb"] = 3;

  PARAM_START("+");
  PARAM_INT("+", "stride", 0, 1);
  m_n_out_layers["+"] = 1;

  PARAM_START("*")
  PARAM_INT("*", "stride", 0, 1);
  m_n_out_layers["*"] = 1;

  PARAM_START("|")
  PARAM_INT("|", "stride", 0, 1);
  m_n_out_layers["|"] = 1;
  
  PARAM_START("|<")
  PARAM_INT("|<", "stride", 0, 1);
  m_n_out_layers["|<"] = 1;

  PARAM_START("+3");
  PARAM_INT("+3", "stride", 0, 3);
  m_n_out_layers["+3"] = 3;

  PARAM_START("*3")
  PARAM_INT("*3", "stride", 0, 3);
  m_n_out_layers["*3"] = 3;

  PARAM_START("|3")
  PARAM_INT("|3", "stride", 0, 3);
  m_n_out_layers["|3"] = 3;
  
  PARAM_START("|<3")
  PARAM_INT("|<3", "stride", 0, 3);
  m_n_out_layers["|<3"] = 3;

  PARAM_START("blur")
  PARAM_INT("blur", "r", 0, 2);
  PARAM_INT("blur", "n", 1, 4);
  PARAM_INT("blur", "m", 2, 0);
  m_n_out_layers["blur"] = 1;

  PARAM_START("light")
  PARAM_FLOAT("light", "lx", 0, 1);
  PARAM_FLOAT("light", "ly", 1, 1);
  m_n_out_layers["light"] = 1;
  
  PARAM_START("trans")
  PARAM_INT("trans", "ox", 0, 128);
  PARAM_INT("trans", "oy", 1, 128);
  PARAM_INT("trans", "tx", 2, 0);
  PARAM_INT("trans", "ty", 3, 0);
  PARAM_FLOAT("trans", "sx", 4, 1);
  PARAM_FLOAT("trans", "sy", 5, 1);
  PARAM_FLOAT("trans", "a", 6, 0.25);
  m_n_out_layers["trans"] = 1;

  PARAM_START("msk")
  PARAM_INT("msk", "m", 0, 0);
  PARAM_FLOAT("msk", "c", 1, 1);
  PARAM_FLOAT("msk", "t", 2, 0.5);
  PARAM_FLOAT("msk", "t2", 3, 1);
  m_n_out_layers["msk"] = 1;

  PARAM_START("mix")
  PARAM_INT("mix", "m", 0, 0);
  m_n_out_layers["mix"] = 1;

  
  PARAM_START("light")
  PARAM_FLOAT("light", "lx", 0, 1);
  PARAM_FLOAT("light", "ly", 1, 1);
  m_n_out_layers["light"] = 1;

  PARAM_START("alpha");
  PARAM_INT("alpha", "stride", 0, 1);
  m_n_out_layers["alpha"] = 1;

  PARAM_START("alpha3");
  PARAM_INT("alpha3", "stride", 0, 3);
  m_n_out_layers["alpha3"] = 3;

  PARAM_START("brick");
  PARAM_INT("brick", "w", 0, 64);
  PARAM_INT("brick", "h", 1, 32);
  PARAM_FLOAT("brick", "c", 2, 1);  
  m_n_out_layers["brick"] = 1;

  m_n_out_layers["out"] = 3;
}

AST_Node* get_symbol_define_variable(AST_Node* sexp, string& var_name) {
  var_name = "";
  AST_Node::iterator it = sexp->begin();
  if ( it == sexp->end() ) return NULL;
  if ( (*it)->Value() == "=") it++; else return NULL;
  if (it != sexp->end()) var_name = (*it)->Value(); else return NULL;
  it++;
  if (it != sexp->end() && (*it)->IsList() ) {
    return *it;
  } else return NULL;
}

bool get_param(const std::string& token, std::string& name, double& val, bool& no_value)
{
  no_value=false;
  std::string split[16];
  //  cout <<"T:" << token<<"\n";
  if ( split_string(token, ":", split, 15) == 1 ) {

    if ( is_token_symbol(split[0]) ) 
      name = split[0];
    else { cout << "Wrong paramater name!: " << split[0].c_str() << "\n"; return false; }
    
    if ( is_token_number(split[1]) )
      val = atof(split[1].c_str());
    else { cout << "Wrong paramater value!: " << split[1].c_str() << "\n"; return false; }

  } else no_value=true; //{cout << "Invalid paramater syntax!: " << token.c_str(); return false; }

  return true;
}

bool TextureGeneration::OrderedParameters(string& op_name, const vector<string>& names, const vector<double>& values, 
					  vector<param>& ordered_params)
{
  map<std::string, map<std::string, Param*>  >::iterator it = m_defined_parameters.find(op_name);
  if ( it != m_defined_parameters.end() ) {
    map<std::string, Param*> params = it->second;
    ordered_params.resize(params.size());
    for ( int paramNameIndex = 0;  paramNameIndex<names.size(); paramNameIndex++ ) {
      map<std::string, Param*>::iterator paramIt = params.find(names[paramNameIndex]);
      if ( paramIt != params.end() ) {
	param p;
	if ( (*paramIt).second->real )
	  p.f = values[paramNameIndex];
	else
	  p.i = values[paramNameIndex];
	ordered_params[(*paramIt).second->index] = p;
	//cout << "Found: " << names[paramNameIndex] << "[" << paramNameIndex << "]" << "->" << values[paramNameIndex] << "\n";
	params.erase(paramIt);
      }
      //else { cout << "Unknown paramater name: " << names[paramNameIndex] << "\n"; return false; }
    }
    for (map<std::string, Param*>::iterator itRestParams = params.begin(); 
	 itRestParams != params.end(); itRestParams++)  {
      ordered_params[(*itRestParams).second->index] = (*itRestParams).second->default_val;
    }
  }
  else { cout << "Unknown operator name: " << op_name.c_str() << "\n"; return false; }
  return true;
}

typedef vector<layer> layer_vec;
vector<layer> TextureGeneration::EvalExpression(map<string, layer_vec> & evaluated_variables, AST_Node* exp)
{
  vector<layer> result;
  if ( exp ) {
    vector<string> param_names;
    vector<double> param_values;
    vector<layer> input_layers;
    AST_Node::iterator it = exp->begin();
    if ( it == exp->end() ) { cout << "Error: Null list variable cannot be defined!\n"; return result; }
    if ( (*it)->IsList() ) { cout << "Error: Operator expected not list!\n"; return result; }
    std::string op_name = (*it)->Value();
    op::gen_func op_func = get_operator((*it)->Value().c_str());
    if ( op_func == NULL ) { cout << "Error: \"" << (*it)->Value().c_str() << "\" is not defined core operator!\n"; return result; }
    it++;
    while (it != exp->end() ) {
      //cout << "OP: " << (*it)->Value().c_str() << "\n";
      if ( !(*it)->IsList() )
	{

	  string name;
	  double val;
	  bool no_value;
	  if ( get_param((*it)->Value(), name, val, no_value) ) {
	    if ( !no_value ) {
	      param_names.push_back(name);
	      param_values.push_back(val);
	    } else {
	      map<std::string, vector<layer> >::iterator foundVar = evaluated_variables.find((*it)->Value());
	      if ( foundVar == evaluated_variables.end() ) { cout << "Error: Variable: \"" << (*it)->Value().c_str() << "\" not defined!\n"; return result; }
	      input_layers.insert(input_layers.end(), (*foundVar).second.begin(), (*foundVar).second.end());
	    }
	    //cout << "\t" << name << ":" << val << "\n";
	  }
	} else {
	//	    if ( (*it)->IsList() ) {
	//cout << "Evaluating S-expr: \n";
	cout.flush();
	vector<layer> nested_results = EvalExpression(evaluated_variables, (*it));
	if ( nested_results.size() == 0 ) {
	  /*cout << "Failed!\n";*/ return result;
	} /*else cout << "Succeeded!:)\n";*/
	input_layers.insert(input_layers.end(), nested_results.begin(), nested_results.end());
      }
      //cout << "****\n";

      it++;
    }
    //}
    vector<param> ordered_params;
    if ( !OrderedParameters(op_name, param_names, param_values, ordered_params) ) return result;
    int n_o_layers = m_n_out_layers[op_name];
    //    if ( n_o_layers == -1) 
    for(int i=0; i<n_o_layers; i++) result.push_back(op::gen_create_layer(256));
    /* cout << "OLayers: " << result.size() << "\n";
    cout << "ILayers: " << input_layers.size() << "\n";
    cout << "Params: " << ordered_params.size() << "\n";
    cout << "OpName: " << op_name << "\n";*/
    op_func(
	    &result[0], m_n_out_layers[op_name],
	    &input_layers[0], input_layers.size(),
	    &ordered_params[0], ordered_params.size(),
	    256);
    //    cout << "xxxx";

  }
  return result;
}

vector<layer> TextureGeneration::DefineOneVariable(map<string, layer_vec >& evaluated_variables, AST_Node* var_def, string& var_name)
{
  var_name = "";
  //cout << "DefineOneVariable>>\n";
  vector<layer> result;
  //  for(int i=0; i<tex_def.size(); i++) {
  AST_Node* exp = get_symbol_define_variable(var_def, var_name);
  //cout << "Variable: " << var_name << " " << exp << "\n";
  return EvalExpression(evaluated_variables, exp);
}

vector<layer> TextureGeneration::Generate(vector<AST_Node*> tex_def) {
  map<std::string, vector<layer> > evaluated_variables;
  for(int i=0; i<tex_def.size(); i++) {
    std::string var_name;
    vector<layer> ls = DefineOneVariable(evaluated_variables, tex_def[i], var_name);
    if ( i == tex_def.size() - 1 ) var_name = "out";
    evaluated_variables.insert(pair<string, vector<layer> >(var_name, ls));
  }
  return evaluated_variables["out"];
}

extern "C" int tga_save(char* filename,char* data,unsigned int data_size,short width,short height);
int parse_main(const char* texdef_name, const char* bmp_name)
{
  ifstream str(texdef_name, ios::in);
  if ( !str ) { cout << "Error: Can't open texdef input file: " << "\"" << texdef_name << "\" !\n"; return -1; }
  string token;
  vector<AST_Node*> sexps = build_AST(str);
  int index=0;
  TextureGeneration tex_gen;
  for(vector<AST_Node*>::iterator it = sexps.begin(); it != sexps.end(); it++) {
    cout << "Expression: " << index++ << "\n";
    string var_name;
    cout << get_symbol_define_variable(*it, var_name) << "\n";
    cout << "Var name: " << var_name.c_str() << "\n";
    print_AST_node(cout, *it, 0);
    cout << "--------------------\n";
  }

  vector<layer> l = tex_gen.Generate(sexps);
  
  if ( l.size() == 0 ) { cout << "Error: Nothing generated!\n"; return -1; }
  static rgba buf[256*256*8];
  if ( l.size() == 1 ) { l.push_back(l[0]); l.push_back(l[0]); }
  
  op::pack_layers((rgba*)&buf[0], 256, (layer)l[0], (layer)l[1], (layer)l[2]);
  //tga_save("sample_gen.tga",(char*)buf,256*256*4,256,256);    
  bmp_write(bmp_name,(int*)buf,256,256);    
  return 0;
}
