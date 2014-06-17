#include <iostream>

#include "MarkovMonoid.hpp"

// Constructor
UnstableMarkovMonoid::UnstableMarkovMonoid(uint dim) : dim(dim)
{
	Matrix::vectors.clear();
	Matrix::zero_vector = &*Matrix::vectors.emplace(0).first;
};

// Cleans up knowns vectors
UnstableMarkovMonoid::~UnstableMarkovMonoid()
{
	Matrix::vectors.clear();
	Matrix::zero_vector = NULL;;
};

// Adds a pair (expression, matrix) in the monoid
void UnstableMarkovMonoid::addElement(const ExtendedExpression * new_expr, const Matrix * new_mat)
{
	expr_to_mat[new_expr] = new_mat;
	mat_to_expr[new_mat] = new_expr;

	elements.push_back(new_expr);
	
	if (expr_to_mat.size() % 10000 == 0)
		cout << expr_to_mat.size() << " elements, " << rewriteRules.size() << " rewrite rules and " << Matrix::vectors.size() << " vectors." << endl;
}

// Adds a rewrite rule
void UnstableMarkovMonoid::addRewriteRule(const ExtendedExpression * pattern, const ExtendedExpression * rewritten)
{
	rewriteRules[pattern] = rewritten;
}

void UnstableMarkovMonoid::addLetter(char a, ExplicitMatrix & mat)
{
	unordered_set<LetterExpr>::const_iterator it = letterExpressions.emplace(a).first;
	unordered_set<Matrix>::const_iterator it2 = matrices.emplace(mat).first;

	const ExtendedExpression * new_expr = &(*it);
	const Matrix * new_mat = &(*it2);
	addElement(new_expr, new_mat);
}

void UnstableMarkovMonoid::process_expression(const ExtendedExpression * elt_left,const ExtendedExpression * elt_right)
{

	// First case: elt_left is a concatenation of expressions
	const ConcatExpr * concatExprLeft = isConcatExpr(elt_left);
	const LetterExpr * LetterExprLeft = isLetterExpr(elt_left);
	const SharpedExpr * SharpedExprLeft = isSharpedExpr(elt_left);
	if (concatExprLeft != NULL)
	{

		bool rewrite_rule_found = false;

		// First subcase: elt_right is a concatenation of expressions
		const ConcatExpr * concatExprRight = isConcatExpr(elt_right);
		const LetterExpr * LetterExprRight = isLetterExpr(elt_right);
		const SharpedExpr * SharpedExprRight = isSharpedExpr(elt_right);
		if (concatExprRight != NULL)
		{
			// TO DO: si left = u_1 ... u_k et right = v_1 ... v_n, il faut vérifier si u_i ... u_k v_1 ... v_j se réduit pour tout i, j.
		}

		// Second and third subcase: elt_right is a letter or a sharped expression
		else 
		{
			ConcatExpr new_concat_expr(elt_right, concatExprLeft->sonsNb + 1);
						
			// For each strict suffix, we test if the expression (which is necessarilly known) is reducible
			for (uint i = 0; i < concatExprLeft->sonsNb - 1; i++)
			{
				const ExtendedExpression * new_e = concatExprLeft->sons[i];
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

			if (!rewrite_rule_found)
			{
				// We compute the matrix
				const Matrix * mat_left = expr_to_mat[elt_left];
				const Matrix * mat_right = expr_to_mat[elt_right];
				Matrix mat = Matrix::prod(* mat_left,* mat_right);
		
				pair <unordered_set<Matrix>::iterator, bool> result = matrices.emplace(mat);

				new_concat_expr.addLeftSon(concatExprLeft->sons[concatExprLeft->sonsNb - 1]);

				unordered_set<ConcatExpr>::iterator it = concatExpressions.emplace(new_concat_expr).first;
				const ConcatExpr * new_expr = &*it;
				if (result.second)
				{
					addElement(new_expr, &(*result.first));
				}
				else
				{
					const ExtendedExpression * rewriten = mat_to_expr[&(*result.first)];
					addRewriteRule(new_expr, rewriten);
				}
			}
		}
	}

	// Second case: elt_left is a letter
	else if (LetterExprLeft != NULL)
	{				
		// First subcase: elt_right is a concatenation of expressions
		const ConcatExpr * concatExprRight = isConcatExpr(elt_right);
		const LetterExpr * LetterExprRight = isLetterExpr(elt_right);
		const SharpedExpr * SharpedExprRight = isSharpedExpr(elt_right);
		if (concatExprRight != NULL)
		{
			// TO DO: si left = a et right = v_1 ... v_n, il faut vérifier si a v_1 ... v_j se réduit pour tout j.			
		}
		// Second subcase: elt_right is a letter
		else if (LetterExprRight != NULL)
		{
			// TO DO: si left = a et right = b, traiter ab
		}

		// Third subcase: elt_right is a sharped expression
		else
		{
			// TO DO: si left = a et right = b^sharp, traiter a b^sharp			
		}
	}
	
	// Third case: elt_left is a sharped expression
	else
	{
		// First subcase: elt_right is a concatenation of expressions
		const ConcatExpr * concatExprRight = isConcatExpr(elt_right);
		const LetterExpr * LetterExprRight = isLetterExpr(elt_right);
		const SharpedExpr * SharpedExprRight = isSharpedExpr(elt_right);
		if (concatExprRight != NULL)
		{
			// TO DO: si left = u^\sharp et right = v_1...v_n, il faut vérifier si u^sharp v_1...v_j se réduit pour tout j			
		}
		// Second subcase: elt_right is a letter
		else if (LetterExprRight != NULL)
		{
			// TO DO: si left = u^\sharp et right = a, traiter u^sharp a						
		}

		// Third subcase: elt_right is a sharped expression
		else
		{
			// TO DO: si left = u^\sharp et right = v^sharp, traiter u^sharp v^sharp									
		}
	}
}

bool UnstableMarkovMonoid::CloseByProduct()
{
	/*  iterer sur tous les elements
		pour chaque element u,
		pour chaque element v,
			on teste si uv est reductible
			si oui -> on fait rien
			si non -> on calcule la matrice, on teste si elle existe deja
				si oui -> on ajoute la nouvelle regle de reecriture
				si non -> on ajoute le nouvel element
	*/

	new_elements.clear();

	size_t i = 0;

	while (i < elements.size())
	{
		const ExtendedExpression *expr_left = elements[i];
		size_t j = 0;
		while (j < elements.size())
		{
			const ExtendedExpression *expr_right = elements[j];
			process_expression(expr_left,expr_right);
			j++;
		}
		i++;
	}

	return (new_elements.size() != 0);
}

void UnstableMarkovMonoid::sharpify_expression(const ExtendedExpression * elt){

	const Matrix * mat_e = expr_to_mat[elt];

	if ((*mat_e).isIdempotent()){
		Matrix mat = Matrix::stab(* mat_e);

		pair <unordered_set <Matrix>::iterator, bool> result = matrices.emplace(mat);
		unordered_set<SharpedExpr>::iterator it = sharpExpressions.emplace(elt).first;

		const SharpedExpr * new_expr = &*it;
		if (result.second){
			addElement(new_expr, &(*result.first));
		}
		else {
			const ExtendedExpression * rewriten = mat_to_expr[&(*result.first)];
			addRewriteRule(new_expr, rewriten);
		}
	}
}

void UnstableMarkovMonoid::CloseByStabilization()
{
	size_t cur_index = 0;

	while (cur_index < new_elements.size())
	{
		const ExtendedExpression *expr = new_elements[cur_index];
		sharpify_expression(expr);
		cur_index++;
	}
}

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

