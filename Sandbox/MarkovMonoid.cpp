#include <iostream>
#include "MarkovMonoid.hpp"

#define VERBOSE_MONOID_COMPUTATION 0

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
		if (canonicalRewriteRules.find(it->first) == canonicalRewriteRules.end())
		{
			it->first->print();
			cout << "  ->  ";
			it->second->print();
			cout << endl;
		}
	}

	cout << "***************************************" << endl;
	cout << "Canonical Rewrite rules " << endl;
	cout << "***************************************" << endl;
	for (map<const ExtendedExpression *, const ExtendedExpression *>::iterator it = rewriteRules.begin(); it != rewriteRules.end(); it++)
	{
		if (canonicalRewriteRules.find(it->first) != canonicalRewriteRules.end())
		{
			it->first->print();
			cout << "  ->  ";
			it->second->print();
			cout << endl;
		}
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
#if VERBOSE_MONOID_COMPUTATION
	cout << "Adding rewrite rule ";
	pattern->print();
	cout << " -> ";
	rewritten->print();
	cout << endl;
#endif
	rewriteRules[pattern] = rewritten;
}

// The main subfunction for ClosureProduct: process an expression
/* if uv is not reducible
	then compute the matrix M,
		if M exists already, 
		then add a rewriting rule,
		else add a new element */
void UnstableMarkovMonoid::process_expression(const ExtendedExpression * elt_left, const ExtendedExpression * elt_right)
{
#if VERBOSE_MONOID_COMPUTATION
			cout << endl << endl << "***************************************" << endl << "Processing: " ;
			(*elt_left).print();
			cout << " and " ;
			(*elt_right).print();
			cout << "." << endl; 
#endif

			// the potentially new concat expression
			// notice that concat expressions are automatically flattened by the constructor,
			// thus the sons os new_expr are necessarilly letter or sharp expressions
			ConcatExpr new_expr(elt_left, elt_right);

			/* this stores whether the expression is reducible or not */
			bool rewrite_rule_found = false;

			//if the expression is already known, we are done
			if (concatExpressions.find(new_expr) != concatExpressions.end())
			{
#if VERBOSE_MONOID_COMPUTATION
				cout << "Expression "; new_expr.print(); cout << " is already known, nothing to do" << endl;
#endif
				return;
			}

			/* We check canonical rewrite rules */
			const ExtendedExpression * canonical_rewrites = NULL;

			/* We check for rule uu# = u#*/
			const SharpedExpr * sh_right = isSharpedExpr(elt_right);
			if (sh_right != NULL && sh_right->son == elt_left)
			{
#if VERBOSE_MONOID_COMPUTATION
				cout << "Adding canonical rewrite rule uu# -> u# for u="; new_expr.print(); cout << endl;
#endif
				canonical_rewrites = sh_right;
			}

			/* We check for rule u#u = u#*/
			const SharpedExpr * sh_left = isSharpedExpr(elt_left);
			if (sh_left != NULL && sh_left->son == elt_right)
			{
#if VERBOSE_MONOID_COMPUTATION
				cout << "Adding canonical rewrite rule u#u -> u# for u="; new_expr.print(); cout << endl;
#endif
				canonical_rewrites = sh_left;
			}

			if (sh_left != NULL && sh_left == sh_right)
			{
#if VERBOSE_MONOID_COMPUTATION
				cout << "Adding canonical rewrite rule u#u# -> u# for u="; new_expr.print(); cout << endl;
#endif
				canonical_rewrites = sh_left;
			}
			if (canonical_rewrites != NULL)
			{
				/* we add the new exprssions to the list of all expressions */
				const ConcatExpr * new_expr = &*concatExpressions.emplace(elt_left, elt_right).first;
				addRewriteRule(new_expr, canonical_rewrites);
				canonicalRewriteRules.insert(new_expr);
				return;
			}


	/* We constitute all possible infixes (for the . operator) of the concatenation of the expressions,
	to check for reducibility according to the current rewrite rules*/
			// we compute the index that separates right sons (at the beginning of new_expr.sons)
			//from left sons (at the end of new_expr.sons)
			const ConcatExpr * ConcatExprRight = isConcatExpr(elt_right);
			int subtrees_nb_right = (ConcatExprRight != NULL) ? ConcatExprRight->sonsNb : 1;



	/* We scan all possible infixes (including the whole concatenation itself) and check whether the infix can be rewritten 
	If the left (resp right) expression is a concatexpression, we scan all sons products starting from right (resp left).
	Otherwise, in case the left (resp right) expression is a letter or a sharp, there is only one son to iterate on.
	*/

	/* we copy data of new_expr */
	const ExtendedExpression ** subtrees = new_expr.sons;
	const uint subtrees_nb = new_expr.sonsNb;


	for (int right_idx = subtrees_nb_right - 1; right_idx >= 0; right_idx--)
	{
		for (int left_idx = subtrees_nb_right; left_idx < subtrees_nb; left_idx++)
		{
			new_expr.sons = subtrees + right_idx;
			new_expr.sonsNb = left_idx - right_idx + 1;
			new_expr.update_hash();
			

			/* if the expression is already known and rewrites, we set rewrite_rule to true*/
			auto expr = concatExpressions.find(new_expr);
#if VERBOSE_MONOID_COMPUTATION
			cout << "Checking for rewrite rule for infix [ " << (subtrees_nb - left_idx) << " : " << (subtrees_nb - right_idx) << " ]"; infix.print();
			cout << " of "; new_expr.print(); cout << endl;
#endif
			if (expr != concatExpressions.end())
			{
				if (rewriteRules.find(&*expr) != rewriteRules.end())
				{
#if VERBOSE_MONOID_COMPUTATION
					cout << ": found rewrite rule "; rewriteRules.find(&*expr)->first->print(); cout << endl;
#endif
					rewrite_rule_found = true;
					break;
				}
#if VERBOSE_MONOID_COMPUTATION
				else
				{
					cout << ": none" << endl;
				}
#endif
			}
		}
		if (rewrite_rule_found) break;
	}
	/* we put new_expr back to normal, memory desallocation will be done by the destructor*/
	new_expr.sons = subtrees;
	new_expr.sonsNb = subtrees_nb;

	// Now, treat if no rewrite rules have been found
	if (!rewrite_rule_found)
	{
		/* we add the new exprssions to the list of all expressions */
		const ConcatExpr * new_expr = &*concatExpressions.emplace(elt_left, elt_right).first;

		/* We compute the matrix */
		Matrix mat = Matrix::prod(*expr_to_mat[elt_left], *expr_to_mat[elt_right]);

		/* we check if the matrix is already known */
		pair <unordered_set<Matrix>::iterator, bool> result = matrices.emplace(mat);
		const Matrix *  new_mat = &(*result.first);
		if (result.second)
		{
#if VERBOSE_MONOID_COMPUTATION
			/* if the matrix was not known before, we create a new association between expression and its matrix */
			cout << "Add element: "; new_expr->print();	 cout << endl;
			cout << "with matrix adress #" << new_mat << " hash h" << new_mat->hash() << endl;
			new_mat->print();
			cout << endl;
#endif
			expr_to_mat[new_expr] = new_mat;
			mat_to_expr[new_mat] = new_expr;

			new_elements.push_back(new_expr);
			to_be_sharpified.push_back(new_expr);
		}
		else
		{
			/* if the matrix was known before, we create the new rewrite rule */
			const ExtendedExpression * rewritten = mat_to_expr[&(*result.first)];
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

/*			cout << "added: " ;
			(*new_expr).print() ;
			cout << endl ; */

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
	} while (new_elements.size() != 0);

}
