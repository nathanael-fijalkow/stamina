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

// adds a pair (expression, matrix) in the monoid
void UnstableMarkovMonoid::addElement(const ExtendedExpression * new_expr, const Matrix * new_mat)
{
	expr_to_mat[new_expr] = new_mat;
	mat_to_expr[new_mat] = new_expr;

	if (expr_to_mat.size() % 10000 == 0)
		cout << expr_to_mat.size() << " elements and " << rewriteRules.size() << " rewrite rules  and " << Matrix::vectors.size() << " vectors." << endl;
}

// adds a rewrite rule
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

	new_elements.push_back(new_expr);
}

void UnstableMarkovMonoid::process_expression(const ExtendedExpression * elt, vector<const ExtendedExpression *> & added_elts)
{
	/* for each letter */
	for (unordered_set<LetterExpr>::iterator it = letterExpressions.begin(); it != letterExpressions.end(); it++)
	{
		const LetterExpr * letterExpr = &(*it);

		const ConcatExpr * concatExpr = isConcatExpr(elt);
		if (concatExpr != NULL)
		{
			ConcatExpr new_concat_expr(letterExpr, concatExpr->sonsNb + 1);

			bool rewrite_rule_found = false;

			/* for each strict suffix, we test if the expression (which is necessarilly known) is reducible */
			for (uint i = 0; i < concatExpr->sonsNb - 1; i++)
			{
				const ExtendedExpression * new_e = concatExpr->sons[i];
				new_concat_expr.addLeftSon(new_e);

				unordered_set<ConcatExpr>::iterator ok = concatExpressions.find(new_concat_expr);
				if (ok != concatExpressions.end())
				{
					const ConcatExpr * uuid = &(*ok);
					map<const ExtendedExpression *, const ExtendedExpression *>::iterator rewrite_rule = rewriteRules.find(uuid);
					/* skip to next letter if a rewrite rule exists */
					if (rewrite_rule != rewriteRules.end())
					{
						rewrite_rule_found = true;
						break;
					}
				}
			}

			if (!rewrite_rule_found)
			{
				// we compute the matrix for the largest suffix
				const Matrix * mat_e = expr_to_mat[elt];
				const Matrix * mat_l = expr_to_mat[letterExpr];
				Matrix mat = Matrix::prod(* mat_e,* mat_l);

				pair <unordered_set<Matrix>::iterator, bool> result = matrices.emplace(mat);

				new_concat_expr.addLeftSon(concatExpr->sons[concatExpr->sonsNb - 1]);
				unordered_set<ConcatExpr>::iterator it = concatExpressions.emplace(new_concat_expr).first;

				const ConcatExpr * new_expr = &*it;

				if (result.second)
				{
					addElement(new_expr, &(*result.first));
					added_elts.push_back(new_expr);
				}
				else
				{
					const ExtendedExpression * rewriten = mat_to_expr[&(*result.first)];
					addRewriteRule(new_expr, rewriten);
				}
			}
		}

		const LetterExpr * leftLetterExpr = isLetterExpr(elt);
		if (leftLetterExpr != NULL)
		{
			// we compute the matrix for the largest suffix
			const Matrix * mat_e = expr_to_mat[elt];
			const Matrix * mat_l = expr_to_mat[letterExpr];
			Matrix mat = Matrix::prod(* mat_e,* mat_l);

			pair <unordered_set<Matrix>::iterator, bool> result = matrices.emplace(mat);

			ConcatExpr new_concat_expr(letterExpr, 2);
			new_concat_expr.addLeftSon(leftLetterExpr);

			unordered_set<ConcatExpr>::iterator it = concatExpressions.emplace(new_concat_expr).first;
			const ConcatExpr * new_expr = &*it;

			if (result.second)
			{
				addElement(new_expr, &(*result.first));
				added_elts.push_back(new_expr);
			}
			else
			{
				addRewriteRule(new_expr, mat_to_expr[&(*result.first)]);
			}
		}
	}
}

bool UnstableMarkovMonoid::CloseByProduct()
{
	/* iterer sur tous les newbies */
	/* pour chaque newbie,
		pour chaque lettre,
		pour chaque suffixe,
		on teste si le suffixe est deja reductible
			si oui-> on fait rien
			si non -> on calcule la matrice, on teste si elle existe deja
				si oui-> on ajoute la nouvelle regle de reecriture
				si non -> on ajoute le nouvel element
				*/

	// elements added at this closure step
	vector<const ExtendedExpression *> very_new_elts;

	size_t cur_index = 0;

	while (cur_index < new_elements.size())
	{
			const ExtendedExpression *expr = new_elements[cur_index];
			int size_before = very_new_elts.size();
			process_expression(expr, very_new_elts);
			new_elements.insert(
				new_elements.end(),
				very_new_elts.begin() + size_before,
				very_new_elts.end()
				);
			cur_index++;
	}

	new_elements = very_new_elts;
	return (very_new_elts.size() != 0);
}

void UnstableMarkovMonoid::sharpify_expression(const ExtendedExpression * elt, vector<const ExtendedExpression *> & added_elts){

	if (true){
//	if (elt.isIdempotent()){
		const Matrix * mat_e = expr_to_mat[elt];
		Matrix mat = Matrix::stab(* mat_e);

		pair <unordered_set <Matrix>::iterator, bool> result = matrices.emplace(mat);

		unordered_set<SharpedExpr>::iterator it = sharpExpressions.emplace(elt).first;

		const SharpedExpr * new_expr = &*it;
		if (result.second){
			addElement(new_expr, &(*result.first));
			added_elts.push_back(new_expr);
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

	// elements added at this closure step
	vector<const ExtendedExpression *> very_new_elts;

	while (cur_index < new_elements.size())
	{
			const ExtendedExpression *expr = new_elements[cur_index];
			sharpify_expression(expr, very_new_elts);
			cur_index++;
	}

	new_elements = very_new_elts;
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

