#include <iostream>
#include "MarkovMonoid.hpp"

// Print
void MarkovMonoid::print()
{
	cout << "***************************************" << endl;
	cout << "Elements " << endl;
	cout << "***************************************" << endl;
	for (map<const ExtendedExpression *, const Matrix *>::iterator it = expr_to_mat.begin(); it != expr_to_mat.end(); it++)
	{
			it->first->print();
			cout << endl;
			it->second->print();
			cout << endl;
	}

	cout << "***************************************" << endl;
	cout << "Rewrite rules " << endl;
	cout << "***************************************" << endl;
	for (map<const ExtendedExpression *, const ExtendedExpression *>::iterator it = rewriteRules.begin(); it != rewriteRules.end(); it++)
	{
		it->first->print();
		cout << "  ->  ";
		it->second->print();
		cout << endl;
	}
}

// Constructor
UnstableMarkovMonoid::UnstableMarkovMonoid(uint dim) : dim(dim)
{
	Matrix::vectors.clear();
	Matrix::zero_vector = &*Matrix::vectors.emplace(0).first;
};

// Free known vectors
UnstableMarkovMonoid::~UnstableMarkovMonoid()
{
	Matrix::vectors.clear();
	Matrix::zero_vector = NULL;;
};

// Adds a letter
void UnstableMarkovMonoid::addLetter(char a, ExplicitMatrix & mat)
{
	unordered_set<LetterExpr>::const_iterator it = letterExpressions.emplace(a).first;
	unordered_set<Matrix>::const_iterator it2 = matrices.emplace(mat).first;

	const ExtendedExpression * new_expr = &(*it);
	const Matrix * new_mat = &(*it2);

	expr_to_mat[new_expr] = new_mat;
	mat_to_expr[new_mat] = new_expr;

	new_elements.push_back(new_expr);
	to_be_sharpified.push_back(new_expr);
}

// Adds a rewrite rule
void UnstableMarkovMonoid::addRewriteRule(const ExtendedExpression * pattern, const ExtendedExpression * rewritten)
{
	rewriteRules[pattern] = rewritten;
}

// The main subfunction for ClosureProduct: process an expression
/* if uv is not reducible
	then compute the matrix M,
		if M exists already, 
		then add a rewriting rule,
		else add a new element */
void UnstableMarkovMonoid::process_expression(const ExtendedExpression * elt_left,const ExtendedExpression * elt_right)
{
		cout << "Processing: " ;
		(*elt_left).print();
		cout << " and " ;
		(*elt_right).print();
		cout << "." << endl;

	cout << "concat expressions known: " ;
	for (const auto& expr: concatExpressions) 
	{
		expr.print();
		cout << " " ;
	}
	cout << endl ;
	
	// Casts
	const ConcatExpr * ConcatExprLeft = isConcatExpr(elt_left);
	const ConcatExpr * ConcatExprRight = isConcatExpr(elt_right);

	bool rewrite_rule_found = false;

	// First case: elt_left and elt_right are concatenations of expressions
	if ((ConcatExprLeft != NULL) && (ConcatExprRight != NULL))
	{
		/* For each prefix v_1 ... v_i of elt_right
		 * 		For each suffix u_j ... u_k of elt_left
		 * 			Test if u_j ... u_k v_1 ... v_i is reducible
		 */
		ConcatExpr new_concat_expr(ConcatExprLeft->sons[0], ConcatExprRight->sonsNb + ConcatExprLeft->sonsNb);

		for (uint i = 1; i <= ConcatExprRight->sonsNb; i++)
		{
			// Compute the prefix v_1 ... v_i 
			ConcatExpr prefix_right_upto_i(i, ConcatExprRight);

			ConcatExpr new_concat_expr(&prefix_right_upto_i, ConcatExprRight->sonsNb + ConcatExprLeft->sonsNb);
			for (uint j = 0; j < ConcatExprLeft->sonsNb; j++)
			{
				const ExtendedExpression * new_e = ConcatExprLeft->sons[j]; 
				new_concat_expr.addLeftSon(new_e);
				unordered_set<ConcatExpr>::iterator ok = concatExpressions.find(new_concat_expr);
				if (ok != concatExpressions.end())
				{
					const ConcatExpr * uuid = &(*ok);
					map<const ExtendedExpression *, const ExtendedExpression *>::iterator rewrite_rule = rewriteRules.find(uuid);
					// skip if a rewrite rule exists
					if (rewrite_rule != rewriteRules.end())
					{
						rewrite_rule_found = true;
						break;
					}
				}
			}
			if (rewrite_rule_found == true) break; 
		}
	}

	// Second case: elt_right is a concatenation of expressions
	else if (ConcatExprRight != NULL)
	{
		// For each strict prefix of elt_right, we test if the expression (which is necessarily known) is reducible
		for (uint i = 1; i < ConcatExprRight->sonsNb; i++)
		{
			// Compute the prefix v_1 ... v_i 
			ConcatExpr prefix_right_upto_i(i, ConcatExprRight);
			ConcatExpr new_concat_expr(&prefix_right_upto_i, ConcatExprRight->sonsNb + 1);
			new_concat_expr.addLeftSon(elt_left);
			
			cout << "check if this rewrites: " ;
			new_concat_expr.print() ;
			cout << endl ;
			
			unordered_set<ConcatExpr>::iterator ok = concatExpressions.find(new_concat_expr);
			if (ok != concatExpressions.end())
			{
				cout << "here1 " ;
				const ConcatExpr * uuid = &(*ok);
				map<const ExtendedExpression *, const ExtendedExpression *>::iterator rewrite_rule = rewriteRules.find(uuid);
				// skip if a rewrite rule exists
				if (rewrite_rule != rewriteRules.end())
				{
				cout << "here2 " ;
					rewrite_rule_found = true;
					break;
				}
			}
		}	
	}

	// Third case: elt_left is a concatenation expression
	else if (ConcatExprLeft != NULL)
	{
		ConcatExpr new_concat_expr(elt_right, ConcatExprLeft->sonsNb + 1);
						
		// For each strict suffix of elt_left, we test if the expression (which is necessarily known) is reducible
		for (uint i = 0; i < ConcatExprLeft->sonsNb - 1; i++)
		{
			const ExtendedExpression * new_e = ConcatExprLeft->sons[i];
			new_concat_expr.addLeftSon(new_e);
			unordered_set<ConcatExpr>::iterator ok = concatExpressions.find(new_concat_expr);
			if (ok != concatExpressions.end())
			{
				const ConcatExpr * uuid = &(*ok);
				map<const ExtendedExpression *, const ExtendedExpression *>::iterator rewrite_rule = rewriteRules.find(uuid);
				// skip if a rewrite rule exists
				if (rewrite_rule != rewriteRules.end())
				{
					rewrite_rule_found = true;
					break;
				}
			}
		}	
	}

// Now, treat if no rewrite rules have been found
	if (!rewrite_rule_found)
	{
		// We compute the matrix
		const Matrix * mat_left = expr_to_mat[elt_left];
		const Matrix * mat_right = expr_to_mat[elt_right];
		Matrix mat = Matrix::prod(* mat_left,* mat_right);
	
		pair <unordered_set<Matrix>::iterator, bool> result = matrices.emplace(mat);
		ConcatExpr new_c_expr(elt_left,elt_right);
		concatExpressions.emplace(new_c_expr);
		
		unordered_set<ConcatExpr>::iterator it = concatExpressions.emplace(new_c_expr).first;
		const ConcatExpr * new_expr = &*it;
	
		if (result.second)
		{
			cout << "Add element: ";
			new_c_expr.print() ;
			cout << endl;

			expr_to_mat[new_expr] = &(*result.first);
			mat_to_expr[&(*result.first)] = new_expr;

			new_elements.push_back(new_expr);
			to_be_sharpified.push_back(new_expr);

		}
		else
		{
			const ExtendedExpression * rewritten = mat_to_expr[&(*result.first)];
			cout << "Add rewrite rule: ";
			(*rewritten).print() ;
			cout << endl;
			addRewriteRule(new_expr, rewritten);
		}
	}
}

void UnstableMarkovMonoid::CloseByProduct()
{
	/* Iterating over all elements
	 *  For every element u,
	 * 		For every element v,
	 * 			process(u,v)
	*/

	size_t i = 0;

	while (i < elements.size())
	{
		const ExtendedExpression * expr_left = elements[i];

		size_t j = 0;
		while (j < new_elements.size())
		{
			const ExtendedExpression * expr_right = new_elements[j];
			process_expression(expr_left,expr_right);
			process_expression(expr_right,expr_left);
			j++;
		}
		i++;			
	}

	i = 0;

	while (i < new_elements.size())
	{
		const ExtendedExpression * expr_left = new_elements[i];

		size_t j = 0;
		while (j < new_elements.size())
		{
			const ExtendedExpression * expr_right = new_elements[j];
			process_expression(expr_left,expr_right);
			j++;
		}
		i++;			
	}
}

// Takes an expression, computes its stabilization if it is idempotent,
// and either add a rewrite rule if the stabilization already exists, or a new element if it does not
void UnstableMarkovMonoid::sharpify_expression(const ExtendedExpression * elt){

	const Matrix * mat_e = expr_to_mat[elt];

	if ((*mat_e).isIdempotent()){
		Matrix mat = Matrix::stab(* mat_e);

		pair <unordered_set <Matrix>::iterator, bool> result = matrices.emplace(mat);
		unordered_set<SharpedExpr>::iterator it = sharpExpressions.emplace(elt).first;
		// DO WE ACTUALLY NEED SHARPEXPRESSIONS (UNORDERED SET)????
		const SharpedExpr * new_expr = &*it;
		if (result.second){
			expr_to_mat[new_expr] = &(*result.first);
			mat_to_expr[&(*result.first)] = new_expr;
			new_elements.push_back(new_expr);
		}
		else {
			const ExtendedExpression * rewritten = mat_to_expr[&(*result.first)];
			addRewriteRule(new_expr, rewritten);
		}
	}
}

// Goes through all the new elements and sharpify them
void UnstableMarkovMonoid::CloseByStabilization()
{
	size_t cur_index = 0;

	while (cur_index < to_be_sharpified.size())
	{
		const ExtendedExpression *expr = to_be_sharpified[cur_index];
		sharpify_expression(expr);
		cur_index++;
	}
}

void UnstableMarkovMonoid::ComputeMarkovMonoid(UnstableMarkovMonoid * monoid)
{
	size_t cur_index = 0;
	while (cur_index < monoid->elements.size())
	{
		new_elements.push_back(monoid->elements[cur_index]);
		to_be_sharpified.push_back(monoid->elements[cur_index]);
		cur_index++;
	}

	int i = 1 ;
	do
	{
		cout << endl << "The current monoid is: " << endl << endl ;

		print() ;
		cout << endl << "Starting iteration " << i << endl << endl ;
		cout << endl << "------> Closure by product" << endl ;
		CloseByProduct();

		cur_index = 0;
		while (cur_index < to_be_sharpified.size())
		{
			elements.push_back(new_elements[cur_index]);
			cur_index++;
		}

		new_elements.clear();	

		cout << "------> Closure by stabilization" << endl << endl ;
		CloseByStabilization() ;
		to_be_sharpified.clear();
		i++;
	} while (to_be_sharpified.size() != 0);

}
